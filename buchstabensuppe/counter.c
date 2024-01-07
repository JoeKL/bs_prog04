#include "counter.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

unsigned char toLowercase[256];

void count(const char *filename)
{

    // Initialisiere die Lookup-Tabelle
    for (int i = 0; i < 256; i++) {
        if (i >= 'A' && i <= 'Z') {
            toLowercase[i] = i - 'A' + 'a';
        } else if (i >= 'a' && i <= 'z') {
            toLowercase[i] = i;
        } else {
            toLowercase[i] = 0; // Nicht-Buchstabe
        }
    }

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
                    if (toLowercase[c] != 0) {
                        // Es ist ein Buchstabe
                        alphabet[toLowercase[c] - 'a']++;
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
