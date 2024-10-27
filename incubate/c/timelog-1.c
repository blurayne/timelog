#include <stdio.h>
#include <time.h>

int main() {
    struct timespec start, current;
    double elapsed;
    int c;

    clock_gettime(CLOCK_MONOTONIC, &start);

    while ((c = getchar()) != EOF) {
        clock_gettime(CLOCK_MONOTONIC, &current);
        elapsed = (current.tv_sec - start.tv_sec) + 
                  (current.tv_nsec - start.tv_nsec) / 1e9;

        int minutes = (int)elapsed / 60;
        double seconds = elapsed - (minutes * 60);

        if (c == '\n') {
            printf("\n[% 3d:%08.5fm] ", minutes, seconds);
        } else {
            putchar(c);
        }
        fflush(stdout);
    }

    return 0;
}
