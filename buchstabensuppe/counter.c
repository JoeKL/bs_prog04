#include "counter.h"
#include <stdio.h>


void count(const char *filename)
{
    FILE *fp;
    char c;

    // Open the file
    fp = fopen(filename, "r");

    // Check if file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    // Extract characters from file and count the occurrences
    while ((c = getc(fp)) != EOF)
    {
        // Check if the character is a letter (uppercase or lowercase)
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            // Convert the character to lowercase (if it's uppercase)
            c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

            // Increment the corresponding counter in the alphabet array
            alphabet[c - 'a']++;
        }
    }

    // Close the file
    fclose(fp);
}