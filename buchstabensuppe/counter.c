#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

// max filesize in kb is 480.000kb == 468.75mb 

#define MAX_QUEUE_SIZE 7500 // 480.000kb / 64kb = 7500
#define BUFFER_SIZE 8192    // each buffer is 64kb = 8192*8 bytes, represented as 8192 * uint64_t
#define NUM_CONSUMERS 6     // number of consumer threads

int finishedReading = 0;

typedef struct
{
    uint64_t buffer[MAX_QUEUE_SIZE][BUFFER_SIZE];
    size_t front, rear;
    pthread_mutex_t queue_mutex;
    pthread_cond_t cond;
} shared_queue_t;

shared_queue_t shared_queue = {
    .front = 0,
    .rear = 0,
    .cond = PTHREAD_COND_INITIALIZER,
    .queue_mutex = PTHREAD_MUTEX_INITIALIZER};

pthread_mutex_t alphabet_mutex;

static inline void processByte(unsigned char *bytePointer, unsigned int *local_ascii, int index)
{
    // if the first two bit of 0b01100000 are set, then we are between 64 and 127
    if ((bytePointer[index] & 192) == 64)
    {
        // then modulos 32 to get the index in the local_ascii array
        local_ascii[bytePointer[index] % 32]++;
    }
}

void *thread_handle_packet()
{
    // local ascii array to store the ascii values of the current buffer
    // using array with length 32 to be able to modulo 32 to get the index in the array
    // this way 1 equals to a, 2 equals to b, etc. and 1 equals to A, 2 equals to B, etc.
    // at the end we just discard entry 0 and entry 27-31
    unsigned int local_ascii[32] = {0};
    while (1)
    {
        if (pthread_mutex_lock(&shared_queue.queue_mutex) != 0)
        {
            perror("pthread_mutex_lock: HERE");
            exit(EXIT_FAILURE);
        }

        // if the queue is empty and we are not done reading
        while (shared_queue.front == shared_queue.rear && !finishedReading)
        {
            if (pthread_cond_wait(&shared_queue.cond, &shared_queue.queue_mutex) != 0)
            {
                perror("pthread_cond_wait:");
                if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0)
                {
                    perror("pthread_mutex_unlock:");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_FAILURE);
            }
        }

        // if the queue is empty and we are done reading
        if (finishedReading && shared_queue.front == shared_queue.rear)
        {
            if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0)
            {
                perror("pthread_mutex_unlock:");
                exit(EXIT_FAILURE);
            }
            break; // break out of while loop to exit thread 
        }

        // get the next bufferelement
        uint64_t *buffer = shared_queue.buffer[shared_queue.front];

        // increment front
        shared_queue.front = (shared_queue.front + 1) % MAX_QUEUE_SIZE;

        if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0)
        {
            perror("pthread_mutex_unlock:");
            exit(EXIT_FAILURE);
        }

        unsigned char *bytePointer;

        // for each entry in the buffer array
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            // if the entry is 0, then the rest of the block is also 0 and we can skip 65536 bytes
            if (buffer[i] == 0)
            {
                break;
            }

            // cast the 8 byte value to a char pointer
            bytePointer = (unsigned char *)&buffer[i];

            // instead of looping over the 8 bytes, we just call inline processByte 8 times to reduce the overhead of the loop
            // since the loop is called 8192 times per buffer in worstcase (when every byte is a letter)
            // it would be called 8192 times per block with 7500 blocks in total = 61.440.000 times
            // averagecase would probs be half of that, so 30.720.000 times
            processByte(bytePointer, local_ascii, 0);
            processByte(bytePointer, local_ascii, 1);
            processByte(bytePointer, local_ascii, 2);
            processByte(bytePointer, local_ascii, 3);

            processByte(bytePointer, local_ascii, 4);
            processByte(bytePointer, local_ascii, 5);
            processByte(bytePointer, local_ascii, 6);
            processByte(bytePointer, local_ascii, 7);
        }
    }

    // when done with all packets, lock alphabet mutex and add local_ascii to alphabet
    if (pthread_mutex_lock(&alphabet_mutex) != 0)
    {
        perror("pthread_mutex_lock:");
        exit(EXIT_FAILURE);
    }

    // add local_ascii to alphabet (except for entry 0 and 27-31)
    for (int i = 1; i <= 26; i++)
    {
        alphabet[i] += local_ascii[i + 1]; // add local_alphabet to alphabet
    }

    if (pthread_mutex_unlock(&alphabet_mutex) != 0)
    {
        perror("pthread_mutex_unlock:");
        exit(EXIT_FAILURE);
    }

    return NULL;
}

void count(const char *filename)
{

    FILE *fp;                            // file pointer
    pthread_t thread_ids[NUM_CONSUMERS]; // array of thread ids

    fp = fopen(filename, "rb"); // open file

    // if file could not be opened
    if (fp == NULL)
    {
        perror("Could not open file");
        return;
    }

    // create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        if (pthread_create(&thread_ids[i], NULL, thread_handle_packet, NULL) != 0)
        {
            // Handle the error. The exact response depends on your application's requirements.
            perror("pthread_create:");
            // You can handle the error based on its type, exit, or retry creating the thread.
            exit(EXIT_FAILURE);
        }
    }

    // read file into queue in 16384 byte chunks until EOF
    while (fread(shared_queue.buffer[shared_queue.rear], 1, BUFFER_SIZE * sizeof(uint64_t), fp) > 0)
    {
        // lock mutex
        if (pthread_mutex_lock(&shared_queue.queue_mutex) != 0)
        {
            perror("pthread_mutex_lock:");
            exit(EXIT_FAILURE);
        }

        // if the queue is not full
        if ((shared_queue.rear + 1) % MAX_QUEUE_SIZE != shared_queue.front)
        {
            shared_queue.rear = (shared_queue.rear + 1) % MAX_QUEUE_SIZE; // increment rear
            if (pthread_cond_signal(&shared_queue.cond) != 0)
            {
                perror("pthread_cond_signal:");
                if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0) // if error in cond signal occurs: unlock mutex
                {
                    
                    perror("pthread_mutex_unlock:");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_FAILURE);
            }

            if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0) // unlock mutex
            {
                perror("pthread_mutex_unlock:");
                exit(EXIT_FAILURE);
            }
        }
        else // if the queue is full
        {
            perror("queue overflow");
            if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0) // unlock mutex
            {
                perror("pthread_mutex_unlock:");
                exit(EXIT_FAILURE);
            }
            return;
        }
    }

    if (fclose(fp) == EOF) // Since fclose doesn't set errno for all errors, setting it manually
    {
        errno = EIO; // I/O error
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_lock(&shared_queue.queue_mutex) != 0) // lock mutex
    {
        perror("pthread_mutex_lock:");
        exit(EXIT_FAILURE);
    }

    finishedReading = 1; // set finishedReading to 1 to signal consumer threads
    if (pthread_cond_broadcast(&shared_queue.cond) != 0) // broadcast signal to consumer threads
    {
        perror("pthread_cond_broadcast:");
        // Handle the error
        if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0)
        {
            perror("pthread_mutex_unlock:");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_unlock(&shared_queue.queue_mutex) != 0) // unlock mutex
    {
        perror("pthread_mutex_unlock:");
        exit(EXIT_FAILURE);
    }

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        if (pthread_join(thread_ids[i], NULL) != 0)
        {
            perror("pthread_join:");
            exit(EXIT_FAILURE);
        }
    }

    // delete 2x LAUCH from the alphabet
    alphabet[11] -= 2; // L == 11
    alphabet[0] -= 2;  // A == 0
    alphabet[20] -= 2; // U == 20
    alphabet[2] -= 2;  // C == 2
    alphabet[7] -= 2;  // H == 7
}