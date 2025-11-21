#include "headers.h"
#include "display.h"
#include "hop.h"
#include "log.h"
#include "reveal.h"
#include "main.h"
#include "process_handler.h"
#include "proclore.h"
#include "seek.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char home_dir[4096];
char prev_dir[4096];
char curr_dir[4096];

char prev_input[4096] = "\0";

char display_prompt_appendable_string[4096];
// Function to trim leading and trailing spaces
char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    end[1] = '\0';

    return str;
}

int split_string(char *str, Command commands[], Command commands2[], int max_commands, int* doesLogExistInTheCommand) {
    int command_count = 0;
    char *segment;
    char *rest = str;

    // First split based on ';'
    while ((segment = strtok_r(rest, ";", &rest))) {
        char *command_part;
        char *command_rest = segment;

        // Now split based on '&' to identify background commands
        while ((command_part = strtok_r(command_rest, "&", &command_rest))) {
            // Trim whitespace from the segment
            command_part = trim_whitespace(command_part);
            //printf("The command part is: %s\n", command_part);
            if (command_part == NULL || command_part[0] == '\0') {
                continue;
            }

            // Check if this is the last part or if there is a '&' after it
            int bgFlag = (*command_rest != '\0');

            // Store the command with the correct background/foreground status
            if (command_count < max_commands) {
                char* check = strdup(command_part); // Duplicate the string
                char* isLogFirstWord = strtok(check, " ");
                if (strcmp(isLogFirstWord, "log") == 0) {
                    *doesLogExistInTheCommand = 1;
                }
                commands[command_count].command = strdup(command_part); // Duplicate the string
                commands2[command_count].command = strdup(command_part);
                commands[command_count].is_background = bgFlag;
                commands2[command_count].is_background = bgFlag;
                command_count++;
            }
            // printf("The command is: %s\n", command_part);
            // printf("and the bgFlag is: %d\n", bgFlag);
            // If the next character in the original string after `command_part` is `&`, it means the current command was a background command
            // if (bgFlag) {
            //     break;  
            // }

        }
    }

    return command_count;
}

int main() {
    // Handling input
    setup_sigchld_handler();
    logStart();
    getcwd(home_dir, sizeof(home_dir));
    strcpy(curr_dir, home_dir);
    strcpy(prev_dir, home_dir);
    strcpy(display_prompt_appendable_string, "");
    // Display logic is first initialised after that it is taken over to the run_command function over in main2
    while (1) {
        // fflush(stdout);
        // setup_sigchld_handler();
                
        // printf("jello : %s\n", display_prompt_appendable_string);
        int isLastProcessBackGround = 0;

        display_prompt(display_prompt_appendable_string);
        strcpy(display_prompt_appendable_string, "");
        char input[4096];
        int doesLogExistInTheCommand = 0;
        bool isTheNewCommandTheSameAsTheOldOne = false;

        // Get the user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Exiting...\n");
            break;  // Exit on EOF or error
        }
        input[strcspn(input, "\n")] = '\0';  // Remove trailing newline character
        char* inputDup = (char*) malloc (sizeof(char) * 4096);
        // strcpy(inputDup, input);
        inputDup = trim_whitespace(input);
        strcpy(input, inputDup);

        char loggableInput[4096];
        strcpy(loggableInput, input);

        if (input[strlen(input) - 1] == '&') {
            isLastProcessBackGround = 1;
            // input[strlen(input) - 1] = '\0';
        }
        // Array to hold the split commands
        Command commands[1024];
        Command commands2[1024];
        int commandCount = split_string(input, commands, commands2, 4096, &doesLogExistInTheCommand);
        if (strcmp(prev_input, "\0") == 0) {
            strcpy(prev_input, input);
        }
        else if (strcmp(prev_input, input) == 0) {
            isTheNewCommandTheSameAsTheOldOne = true;
        }
        else strcpy(prev_input, input);
        // printf("IS previous command same as new: %d\n", isTheNewCommandTheSameAsTheOldOne);
        if (!doesLogExistInTheCommand) {
            logAppend(loggableInput, isTheNewCommandTheSameAsTheOldOne);
            // printf("1: %s\n", loggableInput);
        }

        for (int i = 0; i < commandCount; i++) {
            if (i == commandCount - 1) {
                commands[i].is_background = isLastProcessBackGround;
                commands2[i].is_background = isLastProcessBackGround;
            }
            // printf("Command: %s\n", commands[i].command);
            // printf("Background: %d\n", commands[i].is_background);
        }
        for (int i = 0; i < commandCount; i++) {
            int t = execute_with_timing(commands, commands2, i, prev_dir, isTheNewCommandTheSameAsTheOldOne);
            if (t > 2 && !commands[i].is_background) {
                if (strcmp (display_prompt_appendable_string, "") != 0) {
                    strcat(display_prompt_appendable_string, " ;");
                }
                char* subcommand = strdup(commands[i].command);
                char* firstWord = strtok(subcommand, " ");
                strcat(display_prompt_appendable_string, " ");
                strcat(display_prompt_appendable_string, firstWord);
                strcat(display_prompt_appendable_string, " : ");
                char timeStr[20]; // Ensure this buffer is large enough for the number
                sprintf(timeStr, "%d", t);
                strcat(display_prompt_appendable_string, timeStr);
                strcat(display_prompt_appendable_string, "s");

            }
        }
    }
}