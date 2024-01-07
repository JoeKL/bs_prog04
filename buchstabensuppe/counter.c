#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void count(const char *filename)
{
    FILE *fp;

    // uint64_t buffer; // 64-bit buffer
    uint64_t buffer[512]; // 4096-byte buffer represented as 512 uint64_t
    // unsigned char lauchPattern[] = {0x4C, 0x41, 0x55, 0x43, 0x48, 0x0A}; // "LAUCH" in hexadecimal

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

    // Extract 8 bytes at a time from the file
    while ((fread(buffer, 1, 4096, fp)) > 0)
    {
        bytesRead += 4096;
        for (size_t i = 0; i < 512; ++i)
        {
            if (buffer[i] != 0)
            {

                // Iterate through each byte in the non empty 64-bit buffer
                for (int j = 0; j < 8; ++j)
                {
                    // byteCount++;
                    unsigned char c = (buffer[i] >> (j * 8)) & 0xFF; // Extract each byte from the buffer

                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                    {
                        // Convert the character to lowercase (if it's uppercase)
                        c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

                        // Increment the corresponding counter in the alphabet array
                        alphabet[c - 'a']++;
                    }
                }
            }
            else
            {
                skippedSegments++;
            }
        }
    }
        
    fclose(fp);
    printf("bytes read: %lli\n", bytesRead);
    printf("empty 8Byte segments skipped: %lli; so %lli byte\n", skippedSegments, skippedSegments * 8);
}
