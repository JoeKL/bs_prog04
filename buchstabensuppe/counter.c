#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

// max filesize in kb is 480.000kb == 468.75mb

// #define BLOCK_SIZE 65536        // 64kb da "bs=64K"

#define MAX_QUEUE_SIZE 7500     // 480.000kb / 64kb = 7500
#define BUFFER_SIZE 8192        // each buffer is 64kb = 8192*8 bytes, represented as 8192 * uint64_t
#define NUM_CONSUMERS 8         // number of consumer threads

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

void *thread_handle_packet()
{
    // local ascii array to store the ascii values of the current buffer
    unsigned int local_ascii[32] = {0};

    while (1)
    {
        pthread_mutex_lock(&shared_queue.queue_mutex);

        // if the queue is empty and we are not done reading
        while (shared_queue.front == shared_queue.rear && !finishedReading)
        {
            pthread_cond_wait(&shared_queue.cond, &shared_queue.queue_mutex); // wait for signal from producer
        }

        // if the queue is empty and we are done reading
        if (finishedReading && shared_queue.front == shared_queue.rear)
        {
            pthread_mutex_unlock(&shared_queue.queue_mutex);
            break;
        }

        // get the next bufferelement
        uint64_t *buffer = shared_queue.buffer[shared_queue.front];
        
        // increment front
        shared_queue.front = (shared_queue.front + 1) % MAX_QUEUE_SIZE;

        pthread_mutex_unlock(&shared_queue.queue_mutex); // unlock mutex

        unsigned char *bytePointer;
        int j;

        // for each entry in the buffer array
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            // check if the 8 byte value isnt just 0
            if (buffer[i] != 0)
            {
                // cast the 8 byte value to a char pointer
                bytePointer = (unsigned char *)&buffer[i];

                // Iterate through each byte in the non empty 8-byte buffer
                for (j = 0; j < 8; ++j)
                {
                    // if the first two bit of 0b01100000 are set, then we are between 64 and 127
                    if ((bytePointer[j] & 192) == 64)
                    {
                        // then modulos 32 to get the index in the local_ascii array
                        local_ascii[bytePointer[j] % 32]++;
                    }
                }
            }
        }
    }

    // when done with all packets, lock alphabet mutex and add local_ascii to alphabet

    pthread_mutex_lock(&alphabet_mutex);

    // add local_ascii to alphabet
    for (int i = 0; i < 26; i++)
    {
        alphabet[i] += local_ascii[i + 1]; // add local_alphabet to alphabet
    }

    pthread_mutex_unlock(&alphabet_mutex);

    return NULL;
}

void count(const char *filename)
{

    FILE *fp;  // file pointer
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
        pthread_create(&thread_ids[i], NULL, thread_handle_packet, NULL); // create thread
    }

    // read file into queue in 16384 byte chunks until EOF
    while (fread(shared_queue.buffer[shared_queue.rear], 1, BUFFER_SIZE * sizeof(uint64_t), fp) > 0)
    {
        // lock mutex
        pthread_mutex_lock(&shared_queue.queue_mutex);

        // if the queue is not full
        if ((shared_queue.rear + 1) % MAX_QUEUE_SIZE != shared_queue.front)
        {
            shared_queue.rear = (shared_queue.rear + 1) % MAX_QUEUE_SIZE; // increment rear
            pthread_cond_signal(&shared_queue.cond);         // signal consumer threads
            pthread_mutex_unlock(&shared_queue.queue_mutex); // unlock mutex
        }
        else
        {
            perror("queue overflow");
            pthread_mutex_unlock(&shared_queue.queue_mutex); // unlock mutex
            return;
        }
    }

    fclose(fp); // close file

    pthread_mutex_lock(&shared_queue.queue_mutex);   // lock mutex
    finishedReading = 1;                             // set finishedReading to 1 to signal consumer threads
    pthread_cond_broadcast(&shared_queue.cond);      // broadcast signal to consumer threads
    pthread_mutex_unlock(&shared_queue.queue_mutex); // unlock mutex

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        pthread_join(thread_ids[i], NULL);
    }

    // delete 2x LAUCH from the alphabet
    alphabet[11] -= 2; // L == 11
    alphabet[0] -= 2;  // A == 0
    alphabet[20] -= 2; // U == 20
    alphabet[2] -= 2;  // C == 2
    alphabet[7] -= 2;  // H == 7
}