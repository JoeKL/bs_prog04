#include "counter.h"
#include <stdio.h>
#include <stdint.h>

void count(const char *filename)
{

    FILE *fp;

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    char c;

    // Extract 4096 bytes at a time from the file and store it in the 8*512 array
    while (fread(&c, 1, 1, fp))
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            // Convert the character to lowercase (if it's uppercase)
            c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

            // Increment the corresponding counter in the alphabet array
            alphabet[c - 'a']++;
        }
    }

    fclose(fp);
}
