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
        printf("Could not create output file\n");
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
            long currentPos = ftell(fp); // Save current file position
            // printf("Initial '0x4C' found at position: 0x%ld\n", currentPos - 1);
            
            if (fread(buffer, sizeof(unsigned char), 5, fp) == 5)
            {

                // printf("Buffer read for comparison: ");
                // for (int i = 0; i < 5; i++) {
                //     printf("%02X ", buffer[i]);
                // }
                // printf("\n");

                if (memcmp(buffer, lauchPattern + 1, 4) == 0)
                {
                    printf("'LAUCH' pattern found at position: 0x%lX\n", currentPos - 1);
                    if (lauchDepthCount == 0)
                    {
                        lauchDepthCount += 1; // Set the flag to start saving
                        continue; // Skip the characters in the first "LAUCH" bit pattern
                    }
                    else
                    {
                        // Stop saving when the second "LAUCH" is found
                        lauchDepthCount -= 1; // unset the flag to stop saving
                        break;
                    }
                }
                else
                {
                    // printf("'LAUCH' pattern not found, resetting file pointer to position: 0x%lX\n", currentPos);
                    fseek(fp, -5, SEEK_SET); // Reset file pointer if "LAUCH" is not found
                }
            }
        }

        // Save the current character in the "LAUCH" section
        if (lauchDepthCount)
        {
            fputc(c, outputFile);
            
            // Check if the character is a letter (uppercase or lowercase)
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                // Convert the character to lowercase (if it's uppercase)
                c = (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;

                // Increment the corresponding counter in the alphabet array
                alphabet[c - 'a']++;
            }

            // putchar(c);
        }
    }

    // Close the input and output files
    fclose(fp);

    // fwrite(lauchPattern, sizeof(unsigned char), 5, outputFile); // Write "LAUCH" to the end of output file
    fclose(outputFile); // close output file
}
