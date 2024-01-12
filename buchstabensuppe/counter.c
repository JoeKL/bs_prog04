#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_QUEUE_SIZE 131072 // 2^17
#define BUFFER_SIZE 512       // each buffer is 4096 bytes, represented as 512 uint64_t
#define NUM_CONSUMERS 8       // number of consumer threads

int finishedReading = 0;
int callValue = 0;

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
    unsigned int local_ascii[128] = {0};

    while (1)
    {
        pthread_mutex_lock(&shared_queue.queue_mutex);

        // if the queue is empty and we are not done reading
        while (shared_queue.front == shared_queue.rear && !finishedReading)
        {
            pthread_cond_wait(&shared_queue.cond, &shared_queue.queue_mutex);
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

        pthread_mutex_unlock(&shared_queue.queue_mutex);

        // for each entry in the buffer array
        for (size_t i = 0; i < 512; ++i)
        {
            // check if the 8 byte value isnt just 0
            if (buffer[i] != 0)
            {
                // cast the 8 byte value to a char pointer
                unsigned char *bytePointer = (unsigned char *)&buffer[i];

                // Iterate through each byte in the non empty 8-byte buffer
                for (int j = 0; j < 8; ++j)
                {

                    // Check if the character is a letter (A-Z or a-z) using bit manipulation
                    if ((bytePointer[j] | 32) - 'a' < 26)
                    {
                        local_ascii[bytePointer[j]]++;
                    }
                }
            }
        }
    }

    pthread_mutex_lock(&alphabet_mutex); // lock alphabet mutex

    // add local_alphabet to alphabet
    for (int i = 0; i < 26; i++)
    {
        alphabet[i] += local_ascii[i+'a']; // add local_alphabet to alphabet
        alphabet[i] += local_ascii[i+'A']; // add local_alphabet to alphabet
    }

    pthread_mutex_unlock(&alphabet_mutex);

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
        printf("Could not open file %s", filename);
        return;
    }

    // create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        pthread_create(&thread_ids[i], NULL, thread_handle_packet, NULL);
    }

    // read file into queue in 4096 byte chunks until EOF
    while (fread(shared_queue.buffer[shared_queue.rear], 1, 4096, fp) > 0)
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

    // printf("work done. %li buffers left to read\n", shared_queue.rear - shared_queue.front);

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        pthread_join(thread_ids[i], NULL);
    }

    printf("callValue: %d\n", callValue);

    // delete 2x LAUCH from the alphabet
    alphabet[11] -= 2; // L == 11
    alphabet[0] -= 2;  // A == 0
    alphabet[20] -= 2; // U == 20
    alphabet[2] -= 2;  // C == 2
    alphabet[7] -= 2;  // H == 7
}