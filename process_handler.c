#include "process_handler.h"
#include "headers.h"


#define MAX_BG_PROCESSES 1024

pid_t bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

void setup_sigchld_handler() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;  // Set the handler function
    sigemptyset(&sa.sa_mask);         // Block no additional signals during handler execution
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Restart interrupted system calls; don't notify on stopped children

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int signum) {
    pid_t pid;
    int status;

    // Reap all dead child processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("\nBackground process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("\nBackground process with PID %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }

        // Remove the process from the array
        for (int i = 0; i < bg_process_count; i++) {
            if (bg_processes[i] == pid) {
                for (int j = i; j < bg_process_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_process_count--;
                break;
            }
        }
    }
}

void check_background_processes() {
    int status;
    for (int i = 0; i < bg_process_count; i++) {
        pid_t result = waitpid(bg_processes[i], &status, WNOHANG);
        if (result > 0) { // Process has terminated
            printf("Background process with PID %d has terminated.\n", bg_processes[i]);
            // Remove the process from the array
            for (int j = i; j < bg_process_count - 1; j++) {
                bg_processes[j] = bg_processes[j + 1];
            }
            bg_process_count--;
            i--; // Adjust the index to account for the removed process
        }
    }
}

void handle_fork_and_execute(char* command, int is_background) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the command
        setsid();  // Disassociate the child from the terminal

        char* args[1024];
        int j = 0;
        char* token = strtok(command, " ");
        while (token != NULL) {
            args[j++] = token;
            token = strtok(NULL, " ");
        }
        args[j] = NULL;  // Null-terminate the array of arguments

        if (execvp(args[0], args) == -1) {
            printf("Invalid command.\n");
            //perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        if (is_background) {
            printf("Background process started with PID: %d\n", pid);
            bg_processes[bg_process_count++] = pid;  // Store the background process PID
        } else {
            waitpid(pid, NULL, 0);  // Parent process waits for foreground command to complete
        }
    } else {
        perror("fork");
    }
}
