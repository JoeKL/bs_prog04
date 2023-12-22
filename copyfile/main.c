#include "copyfile.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

int main(int argc, char const *argv[])
{

// ./copyfile 1 testfile testfileout
// 4: ./copyfile, 1, testfile, testfileout, 
    
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

        // Zeit messen starten
        copyUsingBuffer(file_src, file_dest);
        // Zeit messen stoppen
        // Zeit ausgeben

        break;

    case 2: // Kopieren mithilfe eines Speicherabbilds
    
        // Zeit messen starten
        copyUsingMap(file_src, file_dest);
        // Zeit messen stoppen
        // Zeit ausgeben

        break;

    case 3: // Kopieren mithilfe von sendfile(2)

        // Zeit messen starten
        copyUsingSystemCall(file_src, file_dest);
        // Zeit messen stoppen
        // Zeit ausgeben
        
        break;

    default:
        // perror("copyfile/copymode: copymode not supported.");
        fprintf(stderr, "copyfile/copymode: copymode not supported.\n");
        exit(EXIT_FAILURE);
        break;
    }

    return 0;
}
