#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 131072
#define BUFFER_SIZE 512 // each buffer is 4096 bytes, represented as 512 uint64_t
#define NUM_CONSUMERS 10 // number of consumer threads

size_t producedBytes = 0;
_Atomic size_t consumedBytes = 0;

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
    .queue_mutex = PTHREAD_MUTEX_INITIALIZER
};


pthread_mutex_t alphabet_mutex;

void *thread_handle_packet()
{
    unsigned int local_alphabet[26] = {0};

    int handledPackets = 0;

    while (1)
    {
        pthread_mutex_lock(&shared_queue.queue_mutex);

        while (shared_queue.front == shared_queue.rear && !finishedReading) {
            pthread_cond_wait(&shared_queue.cond, &shared_queue.queue_mutex);
        }

        if (finishedReading && shared_queue.front == shared_queue.rear) {
            pthread_mutex_unlock(&shared_queue.queue_mutex);
            break;
        }

        uint64_t *buffer = shared_queue.buffer[shared_queue.front];
        shared_queue.front = (shared_queue.front + 1) % MAX_QUEUE_SIZE;
        handledPackets++;

        pthread_mutex_unlock(&shared_queue.queue_mutex);


        // for each entry in the buffer array
        for (size_t i = 0; i < 512; i++)
        {
            consumedBytes += 8;
            // check if the 8 byte value isnt just 0
            if (buffer[i] != 0)
            {
                // faster if added before for loop
                unsigned char c;

                // Iterate through each byte in the non empty 8-byte buffer
                for (int j = 0; j < 8; j++)
                {
                    // Extract each byte from the 8 byte buffer segment
                    c = ((buffer[i] >> (j * 8)) & 0xFF);

                    // if between 65 and 122
                    if (c >= 'A' && c <= 'z')
                    {
                        // if smaller than 90; convert uppercase to lowercase by adding 32
                        if (c <= 'Z')
                        {
                            c = c + 32;
                        }

                        // if bigger than 97,
                        if (c >= 'a')
                        {
                            local_alphabet[c - 'a']++;
                        }
                    }
                }
            }
        }
    }
    
    printf("handled %i packets\n", handledPackets);

    pthread_mutex_lock(&alphabet_mutex);

    for (int i = 0; i < 26; i++) {
        alphabet[i] += local_alphabet[i];
    }

    pthread_mutex_unlock(&alphabet_mutex);

    return NULL;
}

void count(const char *filename)
{
    FILE *fp;
    pthread_t thread_ids[NUM_CONSUMERS];

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&thread_ids[i], NULL, thread_handle_packet, NULL);
    }

    while (fread(shared_queue.buffer[shared_queue.rear], 1, 4096, fp) > 0) {
        pthread_mutex_lock(&shared_queue.queue_mutex);

        if ((shared_queue.rear + 1) % MAX_QUEUE_SIZE != shared_queue.front) {
            producedBytes += 4096;
            shared_queue.rear = (shared_queue.rear + 1) % MAX_QUEUE_SIZE;

            pthread_cond_signal(&shared_queue.cond);
            pthread_mutex_unlock(&shared_queue.queue_mutex);
        } else {
            perror("queue overflow");
            pthread_mutex_unlock(&shared_queue.queue_mutex);
            return;
        }
    }

    fclose(fp);    

    pthread_mutex_lock(&shared_queue.queue_mutex);
    finishedReading = 1;
    pthread_cond_broadcast(&shared_queue.cond);
    pthread_mutex_unlock(&shared_queue.queue_mutex);


    // printf("work done. %li buffers left to read\n", shared_stack.size);

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // debug
    printf("produced %li\n", producedBytes);
    printf("consumed %li\n", consumedBytes);
}