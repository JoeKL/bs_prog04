#include "counter.h"
#include <stdio.h>
#include <string.h>

void count(const char *filename)
{
    FILE *fp, *outputFile;
    unsigned char c;
    unsigned char lauchPattern[] = {0x4C, 0x41, 0x55, 0x43, 0x48, 0x0A}; // "LAUCH" in hexadecimal

    int lauchDepthCount = 0; // Flag to track if "LAUCH" is found

    // Open the input file
    fp = fopen(filename, "rb");

    // Check if the input file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return;
    }

    // Open the output file to save the "LAUCH" section
    outputFile = fopen("output", "wb");

    // Check if the output file can be created
    if (outputFile == NULL)
    {
        printf("Could not create output file");
        fclose(fp);
        return;
    }

    // Extract characters from the file
    while (fread(&c, sizeof(unsigned char), 1, fp) == 1)
    {
        // Check if "LAUCH" is found
        if (c == lauchPattern[0])
        {
            unsigned char buffer[5];
            if (fread(buffer, sizeof(unsigned char), 5, fp) == 5 && memcmp(buffer, lauchPattern + 1, 4) == 0)
            {

                printf("'%s'", buffer);

                if (lauchDepthCount == 0)
                {
                    // fwrite(lauchPattern, sizeof(unsigned char), 5, outputFile); // Write "LAUCH" to the output file
                    printf("Found first 'LAUCH' at 0x%lX\n", ftell(fp));
                    lauchDepthCount += 1; // Set the flag to start saving
                    continue; // Skip the characters in the first "LAUCH" bit pattern
                }
                else
                {
                    // Stop saving when the second "LAUCH" is found
                    printf("Found second 'LAUCH'. Stopping at 0x%lX\n", ftell(fp));
                    lauchDepthCount -= 1; // unset the flag to stop saving
                    break;
                }
            }
        }

        // Save the current character in the "LAUCH" section
        if (lauchDepthCount)
        {
            // // Check if the character is a letter (uppercase or lowercase)
            // if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            // {
            //     // Convert the character to lowercase (if it's uppercase)
            //     c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

            //     // Increment the corresponding counter in the alphabet array
            //     alphabet[c - 'a']++;
            // }

            fputc(c, outputFile);
            // putchar(c);
        }
    }

    // Close the input and output files
    fclose(fp);

    // fwrite(lauchPattern, sizeof(unsigned char), 5, outputFile); // Write "LAUCH" to the end of output file
    fclose(outputFile); // close output file
}
