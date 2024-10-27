#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>

void print_usage() {
    printf("Usage: program [-p prefix] [-s]\n");
    printf("  -p, --prefix STRING      String to prefix before each linebreak.\n");
    printf("  -s, --show-start-end     Show start and end timestamps with timezone.\n");
}

void print_timestamp(const char *message) {
    char buffer[80];
    struct timeval tv;
    struct tm *tm_info;

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S %Z", tm_info);
    printf("%s: %s\n", message, buffer);
}

int main(int argc, char *argv[]) {
    struct timespec start, current;
    double elapsed;
    int c;
    char *prefix = "";
    int show_start_end = 0;

    int option;
    while ((option = getopt(argc, argv, "p:sh")) != -1) {
        switch (option) {
            case 'p':
                prefix = optarg;
                break;
            case 's':
                show_start_end = 1;
                break;
            case 'h':
            default:
                print_usage();
                return 0;
        }
    }

    if (show_start_end) {
        print_timestamp("Start Time");
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    while ((c = getchar()) != EOF) {
        clock_gettime(CLOCK_MONOTONIC, &current);
        elapsed = (current.tv_sec - start.tv_sec) +
                  (current.tv_nsec - start.tv_nsec) / 1e9;

        int minutes = (int)elapsed / 60;
        double seconds = elapsed - (minutes * 60);

        if (c == '\n') {
            printf("\n%s[% 3d:%08.5fm] ", prefix, minutes, seconds);
        } else {
            putchar(c);
        }
        fflush(stdout);
    }

    if (show_start_end) {
        print_timestamp("End Time");
    }

    return 0;
}
