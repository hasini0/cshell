#include "headers.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "main.h"
#include "proclore.h"
#include "process_handler.h"
#include "seek.h"


// #define MAX_BG_PROCESSES 1024

// pid_t bg_processes[MAX_BG_PROCESSES];
// int bg_process_count = 0;
char absoluteHomeDir[4096];


// void check_background_processes() {
//     int status;
//     for (int i = 0; i < bg_process_count; i++) {
//         pid_t result = waitpid(bg_processes[i], &status, WNOHANG);
//         if (result > 0) { // Process has terminated
//             printf("Background process with PID %d has terminated.\n", bg_processes[i]);
//             // Remove the process from the array
//             for (int j = i; j < bg_process_count - 1; j++) {
//                 bg_processes[j] = bg_processes[j + 1];
//             }
//             bg_process_count--;
//             i--; // Adjust the index to account for the removed process
//         }
//     }
// }

int execute_with_timing(Command commands[], Command commands2[], int i, char* prev_dir, bool isTheNewCommandTheSameAsTheOldOne) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    execute_command(commands, commands2, i, prev_dir, isTheNewCommandTheSameAsTheOldOne);

    gettimeofday(&end, NULL);
    int time_s = (end.tv_sec - start.tv_sec); // + ((end.tv_usec - start.tv_usec) / 1000);
    return time_s;
}

void execute_command(Command commands[], Command commands2[], int i, char* prev_dir, bool isTheNewCommandTheSameAsTheOldOne) {
    check_background_processes();

    char* theFirstWord = strtok(commands[i].command, " ");

    if (strcmp(theFirstWord, "hop") == 0) {
        hop(prev_dir, commands2[i].command);
    } 
    else if (strcmp(theFirstWord, "log") == 0) {
        // Handle log subcommands
        char log_command[1024];
        strcpy(log_command, commands2[i].command);

        char* word = strtok(log_command, " ");
        int idk = 0;
        while (word != NULL) {
            if (strcmp("purge", word) == 0) {
                logPurge();
            }
            else if (strcmp("execute", word) == 0) {
                word = strtok(NULL, " ");
                int index = atoi(word);
                logExecute(index);
            }
            word = strtok(NULL, " ");
            idk++;
        }
        if (idk == 1) {
            logDisplay();
        }
    } 
    else if (strcmp(theFirstWord, "reveal") == 0) {
        revealStart(commands2[i].command);
    }
    else if (strcmp(theFirstWord, "proclore") == 0) {
        int idk = 0;
        char proc_command[1024];
        strcpy(proc_command, commands2[i].command);
        char* word = strtok(proc_command, " ");

        while (word != NULL) {
            idk++;
            if (idk == 2) {
                int pid = atoi(word);
                get_process_info(pid);
            }
            word = strtok(NULL, " ");
        }
        if (idk == 1) {
            pid_t pid = getpid();
            get_process_info(pid);
        }
    }
    else if (strcmp(theFirstWord, "seek") == 0) {
        seek(commands2[i].command);
    }
    else {
        // Handle system commands like emacs, gedit, etc.
        handle_fork_and_execute(commands2[i].command, commands2[i].is_background);
        // else {
        //     printf("Error: Command not found\n");
        // }
    }
}