#include "headers.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Assuming the following variables are defined globally in another source file (e.g., main.c):
// char home_dir[4096];
// char prev_dir[1024];
// char curr_dir[1024];
char hostname[1024];
char* username;

void display_prompt(char* display_prompt_appendable_string) {
    // Fetch the username of the current user
    username = getlogin();
    if (username == NULL) {
        perror("getlogin() error");
        exit(EXIT_FAILURE);
    }

    // Get the hostname of the machine
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname() error");
        exit(EXIT_FAILURE);
    }

    // Get the current working directory and store it in curr_dir
    if (getcwd(curr_dir, sizeof(curr_dir)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    // Display the prompt with a relative path if the current directory is within the home directory
    if (strncmp(curr_dir, home_dir, strlen(home_dir)) == 0) {
        // Display relative path from home directory
        printf("<%s@%s:~%s%s> ", username, hostname, curr_dir + strlen(home_dir), display_prompt_appendable_string);
    } else {
        // Display absolute path
        printf("<%s@%s:%s%s> ", username, hostname, curr_dir, display_prompt_appendable_string);
    }
}
