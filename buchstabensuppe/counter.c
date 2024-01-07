#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

unsigned char toLowercase[256];

void initLookupTable()
{
    // Initialisiere die Lookup-Tabelle
    for (int i = 0; i < 256; i++)
    {
        if (i >= 'A' && i <= 'Z')
        {
            toLowercase[i] = i - 'A' + 'a';
        }
        else if (i >= 'a' && i <= 'z')
        {
            toLowercase[i] = i;
        }
        else
        {
            toLowercase[i] = 0; // Nicht-Buchstabe
        }
    }
}

void count(const char *filename)
{

    initLookupTable();

    FILE *fp;

    uint64_t buffer[512]; // 4096-byte buffer represented as 512 uint64_t

    unsigned long long bytesRead = 0;
    unsigned long long skippedSegments = 0;

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    // Extract 4096 bytes at a time from the file and store it in the 8*512 array
    while ((fread(buffer, 1, 4096, fp)) > 0)
    {
        bytesRead += 4096;

        // for each entry in the buffer array
        for (size_t i = 0; i < 512; ++i)
        {
            // check if the 8 byte value isnt just 0
            if (buffer[i] != 0)
            {
                // Iterate through each byte in the non empty 8-byte buffer
                for (int j = 0; j < 8; ++j)
                {
                    // Extract each byte from the 8 byte buffer segment
                    unsigned char c = (buffer[i] >> (j * 8)) & 0xFF;

                    if (toLowercase[c] != 0)
                    {
                        // Es ist ein Buchstabe
                        alphabet[toLowercase[c] - 'a']++;
                    }
                }
            }
            else
            {
                // count skipped segments up
                skippedSegments++;
            }
        }
    }

    fclose(fp);
    printf("bytes read: %lli\n", bytesRead);
    printf("empty 8-Byte segments skipped: %lli; that are %lli byte\n", skippedSegments, skippedSegments * 8);
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
