#include "copyfile.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    clock_t start, end;
    double cpu_time_used;
    
    if(argc != 4){
        perror("only 3 arguments allowed");
        exit(EXIT_FAILURE);
    }

    char *endptr;
    int copy_mode = strtol(argv[1], &endptr, 10);
    
    // Check if conversion was successful
    if (*endptr != '\0') {
        perror("Argument 1 is not an integer!\n");
        exit(EXIT_FAILURE);
    }

    const char *file_src = argv[2];
    const char *file_dest = argv[3];

    switch (copy_mode)
    {
    case 1: // Kopieren mit Zwischenspeicher

        // Startzeit erfassen
        start = clock();

        copyUsingBuffer(file_src, file_dest);

        // Endzeit erfassen
        end = clock();

        // Berechnung der CPU-Zeit in Sekunden
        cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);

        // Zeit ausgeben
        printf("Mode 1 (copyUsingBuffer): CPU time used: %f ms\n", cpu_time_used);

        break;

    case 2: // Kopieren mithilfe eines Speicherabbilds
    
        // Startzeit erfassen
        start = clock();

        copyUsingMap(file_src, file_dest);

        // Endzeit erfassen
        end = clock();

        // Berechnung der CPU-Zeit in Sekunden
        cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);

        // Zeit ausgeben    
        printf("Mode 2 (copyUsingMap): CPU time used: %f ms\n", cpu_time_used);

        break;

    case 3: // Kopieren mithilfe von sendfile(2)

        // Startzeit erfassen
        start = clock();

        copyUsingSystemCall(file_src, file_dest);

        // Endzeit erfassen
        end = clock();

        // Berechnung der CPU-Zeit in Sekunden
        cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);

        // Zeit ausgeben
        printf("Mode 3 (copyUsingSystemCall): CPU time used: %f ms\n", cpu_time_used);
        
        break;

    default:
        // perror("copyfile/copymode: copymode not supported.");
        fprintf(stderr, "copyfile/copymode: copymode not supported.\n");
        exit(EXIT_FAILURE);
        break;
    }

    return 0;
}
