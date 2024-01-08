#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#define MAX_STACK_SIZE 131072
#define BUFFER_SIZE 512 // each buffer is 4096 bytes, represented as 512 uint64_t
#define NUM_CONSUMERS 4 // number of consumer threads

size_t producedBytes = 0;
_Atomic size_t consumedBytes = 0;

int finishedReading = 0;

typedef struct
{
    uint64_t buffer[MAX_STACK_SIZE][BUFFER_SIZE];
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} shared_stack_t;

shared_stack_t shared_stack = {.size = 0, .mutex = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER};
pthread_mutex_t alphabet_mutex;

void *thread_handle_packet()
{
    unsigned int local_alphabet[26] = {0};

    while (1)
    {
        pthread_mutex_lock(&shared_stack.mutex);

        while (shared_stack.size == 0 && !finishedReading) {
            pthread_cond_wait(&shared_stack.cond, &shared_stack.mutex);
        }

        // If finished reading and no more data to process, exit the loop
        if (finishedReading && shared_stack.size == 0) {
            pthread_mutex_unlock(&shared_stack.mutex);
            break;
        }

        uint64_t *buffer = shared_stack.buffer[shared_stack.size - 1];
        shared_stack.size--;

        pthread_mutex_unlock(&shared_stack.mutex);

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
    pthread_t thread_ids[NUM_CONSUMERS]; // Array to hold thread IDs for consumers

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    // Create multiple consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&thread_ids[i], NULL, thread_handle_packet, NULL);
    }

    // Extract 4096 bytes at a time from the file and store it in the 8*512 array
    while (fread(shared_stack.buffer[shared_stack.size], 1, 4096, fp) != 0) {
        producedBytes += 4096;
        pthread_mutex_lock(&shared_stack.mutex);

        if (shared_stack.size < MAX_STACK_SIZE) {
            shared_stack.size++;
            pthread_cond_signal(&shared_stack.cond);
        } else {
            perror("buffer overflow");
            pthread_mutex_unlock(&shared_stack.mutex);
            return;
        }

        pthread_mutex_unlock(&shared_stack.mutex);
    }

    fclose(fp);    

    // Signal that reading is finished
    pthread_mutex_lock(&shared_stack.mutex);
    finishedReading = 1;
    printf("work done. %li buffers left to read\n", shared_stack.size);
    pthread_cond_broadcast(&shared_stack.cond); // Wake up all waiting threads
    pthread_mutex_unlock(&shared_stack.mutex);

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Cleanup
    printf("produced %li\n", producedBytes);
    printf("consumed %li\n", consumedBytes);
}