#include "copyfile.h"
#include <stdio.h>
#include <stdlib.h>

int copyUsingBuffer(const char *file_src, const char *file_dest)
{
    printf("Mode: 1: Kopieren mit Zwischenspeicher\n");
    printf("copy %s to %s\n", file_src, file_dest);

    // 1. Quell und Zieldatei mittels open(2) öffnen
    // 2. geeignete Flags beim Öffnen wählen: Quelldatei nur lesen und Zieldatei ggf. erstellen und nur beschreiben
    // 3. Puffer mithilfe von malloc(3) allokieren
    // 4. Größe der Datei fstat(2) ermitteln
    // 5. Mit read(2) und write(2) Aufrufe den Puffer von der Quelldatei befüllen und in die Zieldatei schreiben 
    // 6. beide Dateien mit close(2) schließen
    // 7. Puffer mit free(3) freigeben

    // im Nachgang: Fehler abfangen

    return 0;
}

int copyUsingMap(const char *file_src, const char *file_dest)
{
    printf("Mode: 1: Kopieren mithilfe eines Speicherabbilds\n");
    printf("copy %s to %s\n", file_src, file_dest);

    // 1. Quell und Zieldatei mittels open(2) öffnen
    // 2. geeignete Flags beim Öffnen wählen: Quelldatei nur lesen und Zieldatei ggf. erstellen und nur beschreiben
    // 3. Größe der Datei fstat(2) ermitteln
    // 4. die Quelldatei mithilfe von mmap(2) in den Speicher einblenden 
    // 5. mit write(2) direkt von dem eingeblendeten Speicher in die Zieldatei schreiben
    // 6. beide Dateien mit close(2) schließen
    // 7. Speicherabbild mit munmap(2) wieder freigeben

    // im Nachgang: Fehler abfangen


    return 0;
}

int copyUsingSystemCall(const char *file_src, const char *file_dest)
{
    printf("Mode: 1: Kopieren mithilfe von sendfile(2)\n");
    printf("copy %s to %s\n", file_src, file_dest);

    // 1. Quell und Zieldatei mittels open(2) öffnen
    // 2. geeignete Flags beim Öffnen wählen: Quelldatei nur lesen und Zieldatei ggf. erstellen und nur beschreiben
    // 3. Größe der Datei fstat(2) ermitteln
    // 4. mit sendfile(2)-Aufruf die ganze Datei kopieren
    // 5. nach dem copy die Dateien wieder schließen

    // im Nachgang: Fehler abfangen

    return 0;
}