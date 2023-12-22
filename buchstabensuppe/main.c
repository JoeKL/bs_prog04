#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "counter.h"

size_t alphabet[26] = {0};

int main(void)
{
    // start time
    struct timeval start, end, dur;
    gettimeofday(&start, NULL);

    // bench
    count("teller");

    // end bench
    gettimeofday(&end, NULL);
    timersub(&end, &start, &dur);

    // print results
    for (int i = 0; i < 26; ++i)
    {
        printf("%c: %lu\n", (char)(i + 97), alphabet[i]);
    }
    printf("Time: %lu us\n", (dur.tv_sec * 1000000 + dur.tv_usec));

    return 0;
}
