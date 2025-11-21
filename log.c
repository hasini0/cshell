#include "headers.h"
#include "log.h"
#include "main.h"
#define MAX_LEN 15


// IF THERE IS A LOG EXECUTE WITH SLEEP 3 IN FG THEN THE LOG DOES NOT UPDATE THE DISPLAY SYSTEM TOO.

// Concatenating home directory with the logfile name
char logfilename[4096];

#include <stdio.h>

int calculateLineCount(const char *filePath) {
    FILE *file = fopen(filePath, "r"); // Open the file in read mode
    if (file == NULL) {
        perror("Unable to open the file");
        return -1; // Return -1 if the file couldn't be opened
    }

    int count = 0;
    char buffer[1024]; // Buffer to store each line read from the file

    // Read the file line by line
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        count++; // Increment count for each line
    }

    fclose(file); // Close the file
    return count; // Return the total number of lines
}

void logStart() {
    getcwd(logfilename, sizeof(logfilename));
    strcat(logfilename, "/logfile.txt");

    FILE *file = fopen(logfilename, "r");
    if (file == NULL) {
        // Create the file if it doesn't exist
        file = fopen(logfilename, "w");
        if (file == NULL) {
            perror("Error creating logfile");
            return;
        }
    }
    fclose(file);
}

void logAppend(char* command, bool isTheNewCommandTheSameAsTheOldOne) {
    if (isTheNewCommandTheSameAsTheOldOne) {
        return;
    }
    else {
        FILE *file = fopen(logfilename, "r");
        if (file == NULL) {
            perror("Error opening logfile");
            return;
        }
        int line_count = 0;

        // Read all lines into an array
        char *lines[MAX_LEN];
        char buffer[1024];
        line_count = 0; // Reset line_count

        while (fgets(buffer, sizeof(buffer), file)) {
            if (line_count < MAX_LEN) {
                lines[line_count] = strdup(buffer);
                line_count++;
            }
        }
        fclose(file); // Close the file after reading

        // If there are 15 lines, remove the first line
        if (line_count >= MAX_LEN) {
            free(lines[0]);  // Free the memory of the first line
            for (int i = 1; i < line_count; i++) {
                lines[i - 1] = lines[i];
            }
            line_count--;  // Decrease the line count
        }

        // Add the new line (with a newline character)
        lines[line_count] = (char *)malloc(strlen(command) + 2);  // Allocate space for the command and '\n'
        sprintf(lines[line_count], "%s\n", command);  // Add the command followed by a newline
        line_count++;

        // Write the lines back to the file
        file = fopen(logfilename, "w");
        if (file == NULL) {
            perror("Error opening logfile");
            return;
        }

        for (int i = 0; i < line_count; i++) {
            fprintf(file, "%s", lines[i]);
            free(lines[i]);  // Free the memory after writing the line
        }

        fclose(file);
    }
}

void logPurge() {
    FILE *file = fopen(logfilename, "w");
    if (file == NULL) {
        perror("Error opening logfile");
        return;
    }
    fclose(file);
}

void logDisplay() {
    FILE *file = fopen(logfilename, "r");
    if (file == NULL) {
        perror("Error opening logfile");
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void logExecute(int index) {
    FILE *file = fopen(logfilename, "r");
    if (file == NULL) {
        perror("Error opening logfile");
        return;
    }
    char line[1024];
    int count = calculateLineCount(logfilename);

    if (index <= 0 || index > count) {
        printf("Invalid index: %d. Please provide a valid line number.\n", index);
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (count == index) {
            printf("Executing: %s", line);

            Command commands[1024];
            Command commands2[1024];
            int commandCount = split_string(line, commands, commands2, 1024, 0);

            for (int i = 0; i < commandCount; i++) {
                execute_command(commands, commands2, i, curr_dir, 0);
            }
            break;
        }
        count--;
    }
    fclose(file);
}