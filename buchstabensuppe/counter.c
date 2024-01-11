#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>



void count(const char *filename)
{

    FILE *fp;

    uint64_t buffer[512]; // 4096-byte buffer represented as 512 uint64_t

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    // Extract 4096 bytes at a time from the file and store it in the 8*512 array
    while ((fread(buffer, 1, 4096, fp)) != 0)
    {
        // bytesRead += 4096;

        // for each entry in the buffer array
        for (size_t i = 0; i < 512; i++)
        {

            // check if the 8 byte value isnt just 0 (empty)
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
                        if (c <= 'Z') {
                            c = c + 32;
                        }

                        // if bigger than 97, 
                        if (c >= 'a')
                        {
                            alphabet[c - 'a']++;
                        }
                    }
                }
            }
        }
    }

    fclose(fp);
    // printf("bytes read: %lli\n", bytesRead);
}

// Naive implementation to compare results
void count_naive(const char *filename) {
    FILE *fp;
    int counts[26] = {0};

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return;
    }

    // Read the file one character at a time
    int c;
    while ((c = fgetc(fp)) != EOF) {
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
                counts[c - 'a']++;
            }
        }
    }

    // Print the counts
    for (int i = 0; i < 26; i++) {
        printf("%c: %d\n", 'a' + i, counts[i]);
    }

    fclose(fp);
}