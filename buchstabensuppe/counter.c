#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

// mm265 bytehistogramm

void count(const char *filename)
{

    FILE *fp;

    uint64_t buffer[512]; // 4096-byte buffer represented as 512 uint64_t

    // unsigned long long bytesRead = 0;

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

// void count(const char *filename)
// {

//     FILE *fp;

//     // Open the input file
//     fp = fopen(filename, "rb");

//     // Check if the input file exists
//     if (fp == NULL)
//     {
//         printf("Could not open file %s", filename);
//         return;
//     }

//     char c;

//     // Extract 4096 bytes at a time from the file and store it in the 8*512 array
//     while (fread(&c, 1, 1, fp))
//     {
//         if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
//         {
//             // Convert the character to lowercase (if it's uppercase)
//             c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

//             // Increment the corresponding counter in the alphabet array
//             alphabet[c - 'a']++;
//         }
//     }

//     fclose(fp);
// }
