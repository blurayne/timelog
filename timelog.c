#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <termios.h>
#include <limits.h>
#include <stdbool.h>

#define BUFFER_SIZE 120
#define NOT_FOUND -1

int strpos(const char *haystack, const char *needle, int offset) {
    char *p;
    size_t len, pos;

    len = strlen(haystack);
    pos = 0;

    if (offset < 0) {
        if (len > INT_MAX || offset < -(int)len) {
            pos = len + offset;
        }
    } else {
        if (len <= INT_MAX && offset > len) {
            return -1; // Return -1 instead of NULL
        }
        pos = offset;
    }

    p = strstr(haystack + pos, needle);
    if (p != NULL && p - haystack <= INT_MAX) {
        return (int)(p - haystack);
    } else {
        return -1; // Return -1 for not found
    }
}

void print_usage() {
    printf("Usage: program [-p prefix] [-s]\n");
    printf("  -p, --prefix STRING      String to prefix before each linebreak.\n");
    printf("  -s, --show-start-end     Show start and end timestamps with timezone.\n");
}

char* get_timestamp() {
    static char buffer[80];
    struct timeval tv;
    struct tm *tm_info;

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S %Z", tm_info);

    return buffer;
}

char* get_elapsed_time(struct timespec *start) {
    struct timespec current;
    static char output[32];
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &current);

    elapsed = (current.tv_sec - start->tv_sec) +
              (current.tv_nsec - start->tv_nsec) / 1e9;

    int minutes = (int)elapsed / 60;
    double seconds = elapsed - (minutes * 60);
    sprintf(output, "[%3d:%08.5fm]", minutes, seconds);

    return output;
}

void set_terminal_mode(int enable) {
    static struct termios oldt, newt;
    if (!enable) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    } else {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
}

int main(int argc, char *argv[]) {
    struct timespec start;
    char buffer[BUFFER_SIZE];
    char *prefix = "";
    int show_start_end = 0;
    int has_input = 0;

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    fd_set readfds;
    struct timeval timeout;

    // Check if stdin is a terminal
    if (isatty(fileno(stdin))) {
        print_usage();
        return 0;
    }

    if (show_start_end) {
        printf("%s%s Started at %s\n", prefix, get_elapsed_time(&start), get_timestamp());
    }

    // set_terminal_mode(1); // Disable buffering

    bool ends_with_newline = true;
    bool starts_with_newline = false;
    bool ended_with_newline = false;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 0.1 seconds
        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        if (result == -1) {
            perror("select");
            break;
        } else if (result == 0) {
            // Timeout occurred, continue loop
            continue;
        } else if (FD_ISSET(STDIN_FILENO, &readfds)) {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                ended_with_newline = ends_with_newline;
                ends_with_newline = (bytes_read > 0 && buffer[bytes_read-1] == '\n');
                starts_with_newline = (bytes_read > 0 && buffer[0] == '\n');
                char *line = strtok(buffer, "\n");
                if (starts_with_newline == true) {
                    printf("\n");
                    ended_with_newline = true;
                }
                while (line != NULL) {
                    int token_length = strlen(line);
                    if (ended_with_newline == false) {
                        printf("%s", line);
                        ended_with_newline = true;
                    } else {
                        printf("%s%s %s", prefix, get_elapsed_time(&start), line);
                    }
                    line = strtok(NULL, "\n");
                    // as long as there is a next token
                    if (line != NULL) {
                        printf("\n");
                    }
                }
                if (ends_with_newline == true) {
                    printf("\n");
                    ends_with_newline = true;
                }
                fflush(stdout);
            } else {
                break; // End of input
            }
        }
    }

    // set_terminal_mode(0); // Restore terminal settings

    if (show_start_end) {
        printf("%s%s Ended at %s\n", prefix, get_elapsed_time(&start), get_timestamp());
    }

    return 0;
}
