#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXLEN 1000

char operators[100];
int fds[100][2];
int count = 0;

// Function to set up child process and execute arithmetic operations
int child(int c) {
    close(0);
    dup(fds[2 * c][0]);
    close(3);
    dup(fds[2 * c + 1][0]);
    close(1);
    dup(fds[2 * c + 2][1]);

    if (operators[c] == '+') {
        execl("./add", "add", "-v", NULL);
    } else if (operators[c] == '-') {
        execl("./subtract", "subtract", "-v", NULL);
    } else if (operators[c] == '*') {
        execl("./multiply", "multiply", "-v", NULL);
    } else if (operators[c] == '/') {
        execl("./divide", "divide", "-v", NULL);
    }

    fputs("I hope you don't see me", stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    char line[MAXLEN], *temp;

    FILE *dataFile = fopen(argv[1], "r");
    // Read the first line - it contains the configuration
    fgets(line, MAXLEN, dataFile);

    // Sample content for line: a + b - c
    strtok(line, " \n");

    while (temp = strtok(NULL, " \n")) {
        operators[count] = temp[0];
        printf("operator: %c\n", operators[count]);
        count++; // Tracks the number of operators
        strtok(NULL, " \n"); // Skip the symbol representing variable/parameter
    }

    for (int i = 0; i < ((2 * count) + 1); i++) {
        pipe(fds[i]); // Create pipes
    }

    // Setup the configuration with the necessary number of children
    for (int i = 0; i < count; i++) {
        if (fork() == 0) {
            child(i);
        }
    }

    // Read and process data
    int x, z;
    while (fscanf(dataFile, "%d", &x) > 0) {
        if (write(fds[0][1], &x, sizeof(int)) > 0) {
            for (int i = 0; i < count; i++) {
                fscanf(dataFile, "%d", &x);
                if (write(fds[2 * i + 1][1], &x, sizeof(int)) == 0) {
                    exit(0);
                }
            }
            if (read(fds[2 * count][0], &z, sizeof(int)) > 0) {
                printf("%d\n", z);
            }
        }
    }

    fclose(dataFile);
    return 0;
}
