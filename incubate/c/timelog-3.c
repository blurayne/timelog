#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_PREFIX "PREFIXSTRING"
#define BUFFER_SIZE 1024

void show_help() {
    printf("Usage: program_name [options]\n");
    printf("Options:\n");
    printf("  -h          Show help information.\n");
    printf("  -P STRING   Specify a custom prefix string.\n");
}

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];  // Buffer to hold input
    size_t bytesRead;
    char *prefix = DEFAULT_PREFIX; // Default prefix

    // Check for command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            show_help();
            return 0;
        } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
            prefix = argv[++i];
        }
    }

    // Check if stdin is a terminal
    if (isatty(fileno(stdin))) {
        show_help();
        return 0;
    }

    // Read from stdin until EOF
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE - 1, stdin)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        // Iterate through the buffer character by character
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            printf("\n%s%s", prefix, line);
            fflush(stdout);
            line = strtok(NULL, "\n");
        }    

        // fflush(stdout); // Flush the output after processing the buffer
    }

    // Check for errors
    if (ferror(stdin)) {
        perror("Error reading from stdin");
    }

    return 0;
}
