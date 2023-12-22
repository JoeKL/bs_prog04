#include "copyfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/mman.h>
#include <sys/sendfile.h>

int copyUsingBuffer(const char *file_src, const char *file_dest)
{

    if(file_src == file_dest){
        perror("file_src can't be equal to file_dest!");
        exit(EXIT_FAILURE);
    }

    // Quell und Zieldatei mittels open(2) öffnen und geeignete Flags beim Öffnen wählen: 
    // Quelldatei nur lesen
    // Zieldatei ggf. erstellen und nur beschreiben

    // O_RDONLY: Nur lesen
    int src_fd = open(file_src, O_RDONLY);
    if (src_fd == -1)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // O_WRONLY: Nur schreiben
    // O_CREAT: erstellen falls Datei nicht existiert 
    // O_TRUNC: falls Datei schon exisitert, dann wird ihr Inhalt gelöscht
    // S_IRUSR: Leserechte für den User.
    // S_IWUSR: Schreibrechte für den User.
    // S_IRGRP: Leserechte für die Gruppe.
    // S_IROTH: Leserechte für andere (also Benutzer, die weder Eigentümer noch in der Gruppe sind).
    int dest_fd = open(file_dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // Writeonly, create if not exists
    if (dest_fd == -1)
    {
        perror("Error opening destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // Puffer mithilfe von malloc(3) allokieren
    const size_t buffer_size = 4096; 
    char *buffer = malloc(buffer_size);
    if (buffer == NULL)
    {
        perror("Error allocating buffer");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }

    // Größe der Datei fstat(2) ermitteln
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1)
    {
        perror("Error getting source file size");
        free(buffer);
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }

    // Mit read(2) und write(2) Aufrufe den Puffer von der Quelldatei befüllen und in die Zieldatei schreiben 
    ssize_t bytesRead, bytesWritten;

    while ((bytesRead = read(src_fd, buffer, buffer_size)) > 0)
    {
        bytesWritten = write(dest_fd, buffer, bytesRead);

        if (bytesWritten == -1)
        {
            perror("Error writing to destination file");
            free(buffer);
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    // beide Dateien mit close(2) schließen
        if (close(src_fd) == -1 || close(dest_fd) == -1)
    {
        perror("Error closing files");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Puffer mit free(3) freigeben
    free(buffer);

    return 0;
}

int copyUsingMap(const char *file_src, const char *file_dest)
{

    if(file_src == file_dest){
        perror("file_src can't be equal to file_dest!");
        exit(EXIT_FAILURE);
    }

    // Quell und Zieldatei mittels open(2) öffnen und geeignete Flags beim Öffnen wählen: 
    // Quelldatei nur lesen
    // Zieldatei ggf. erstellen und nur beschreiben

    // O_RDONLY: Nur lesen
    int src_fd = open(file_src, O_RDONLY);
    if (src_fd == -1)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // O_WRONLY: Nur schreiben
    // O_CREAT: erstellen falls Datei nicht existiert 
    // O_TRUNC: falls Datei schon exisitert, dann wird ihr Inhalt gelöscht
    // S_IRUSR: Leserechte für den User.
    // S_IWUSR: Schreibrechte für den User.
    // S_IRGRP: Leserechte für die Gruppe.
    // S_IROTH: Leserechte für andere (also Benutzer, die weder Eigentümer noch in der Gruppe sind).
    int dest_fd = open(file_dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dest_fd == -1)
    {
        perror("Error opening destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // Größe der Datei fstat(2) ermitteln
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1)
    {
        perror("Error getting source file size");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }    
    
    // Die Quelldatei mithilfe von mmap(2) in den Speicher einblenden
    void *src_map = mmap(NULL, src_stat.st_size, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_map == MAP_FAILED)
    {
        perror("Error mapping source file into memory");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }


    // mit write(2) direkt von dem eingeblendeten Speicher in die Zieldatei schreiben
    ssize_t bytesWritten = write(dest_fd, src_map, src_stat.st_size);
    if (bytesWritten == -1)
    {
        perror("Error writing to destination file");
        munmap(src_map, src_stat.st_size);
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }


    // 5. Beide Dateien mit close(2) schließen
    if (close(src_fd) == -1 || close(dest_fd) == -1)
    {
        perror("Error closing files");
        munmap(src_map, src_stat.st_size);
        exit(EXIT_FAILURE);
    }

    // 6. Speicherabbild mit munmap(2) wieder freigeben
    if (munmap(src_map, src_stat.st_size) == -1)
    {
        perror("Error unmapping source file from memory");
        exit(EXIT_FAILURE);
    }


    return 0;
}

int copyUsingSystemCall(const char *file_src, const char *file_dest)
{

    if(file_src == file_dest){
        perror("file_src can't be equal to file_dest!");
        exit(EXIT_FAILURE);
    }

    // Quell und Zieldatei mittels open(2) öffnen und geeignete Flags beim Öffnen wählen: 
    // Quelldatei nur lesen
    // Zieldatei ggf. erstellen und nur beschreiben

    // O_RDONLY: Nur lesen
    int src_fd = open(file_src, O_RDONLY);
    if (src_fd == -1)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // O_WRONLY: Nur schreiben
    // O_CREAT: erstellen falls Datei nicht existiert 
    // O_TRUNC: falls Datei schon exisitert, dann wird ihr Inhalt gelöscht
    // S_IRUSR: Leserechte für den User.
    // S_IWUSR: Schreibrechte für den User.
    // S_IRGRP: Leserechte für die Gruppe.
    // S_IROTH: Leserechte für andere (also Benutzer, die weder Eigentümer noch in der Gruppe sind).
    int dest_fd = open(file_dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dest_fd == -1)
    {
        perror("Error opening destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // Größe der Datei fstat(2) ermitteln
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1)
    {
        perror("Error getting source file size");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }    
    
    // Mit sendfile(2)-Aufruf die ganze Datei kopieren
    off_t offset = 0;
    ssize_t bytesCopied = sendfile(dest_fd, src_fd, &offset, src_stat.st_size);
    if (bytesCopied == -1)
    {
        perror("Error copying file using sendfile");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }

    // Nach dem Kopieren die Dateien wieder schließen
    if (close(src_fd) == -1 || close(dest_fd) == -1)
    {
        perror("Error closing files");
        exit(EXIT_FAILURE);
    }

    return 0;
}