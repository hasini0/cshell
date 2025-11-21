#include "headers.h"
#include "hop.h"

void initialize_home_directory() {
    if (getcwd(home_dir, sizeof(home_dir)) != NULL) {
        printf("Current Working Directory: %s\n", home_dir);
        strcpy(curr_dir, home_dir);  // Initialize curr_dir to home_dir
        strcpy(prev_dir, home_dir);  // Initialize prev_dir to home_dir
    } else {
        perror("getcwd() error");
        exit(1);
    }
}

char* getparentDir(char* curr_dir) {
    char* parent_dir = (char*)malloc(1024);
    strcpy(parent_dir, curr_dir); // Copy current directory to parent_dir

    int i = strlen(parent_dir) - 1;
    while (i >= 0 && parent_dir[i] != '/') {
        i--;
    }
    if (i >= 0) {
        parent_dir[i] = '\0'; // Truncate to get the parent directory
    }
    return parent_dir;
}

void swap_strings(char str1[1024], char str2[1024]) {
    char temp[1024];
    strcpy(temp, str1);
    strcpy(str1, str2);
    strcpy(str2, temp);
}

int hop(char* prev_dir, char* command) {
    char* current_parsed;
    int count = 0;
    char* commands_list[4096];

    // Tokenize the command
    current_parsed = strtok(command, " \t\n");
    while (current_parsed != NULL) {
        commands_list[count] = current_parsed;
        count++;
        current_parsed = strtok(NULL, " \t\n");
    }

    if (count == 1) { // No command, default to home directory
        if (chdir(home_dir) == 0) {
            strcpy(prev_dir, curr_dir);
            strcpy(curr_dir, home_dir);
            getcwd(curr_dir, sizeof(curr_dir));
            printf("Changed to home directory: %s\n", curr_dir);
        } else {
            // printf("Error: directory doesn't exist\n");
            return 1;
        }
    } else {
        for (int i = 0; i < count; i++) {
            if ((commands_list[i][0]) == '-') {
                if (strlen(commands_list[i]) == 1) {
                    if (chdir(prev_dir) == 0) {
                        swap_strings(prev_dir, curr_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Swapped to previous directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                } else {
                    char new_dir[4096] = "\0";
                    strcat(new_dir, prev_dir);
                    strcat(new_dir, "/");
                    strcat(new_dir, commands_list[i] + 1); // Append the argument

                    if (chdir(new_dir) == 0) {
                        strcpy(prev_dir, curr_dir);
                        strcpy(curr_dir, new_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Changed to directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                }
            } else if (commands_list[i][0] == '~') {
                if (strlen(commands_list[i]) == 1) {
                    if (chdir(home_dir) == 0) {
                        strcpy(prev_dir, curr_dir);
                        strcpy(curr_dir, home_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Changed to home directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                } else {
                    char new_dir[4096] = "\0";
                    strcat(new_dir, home_dir);
                    strcat(new_dir, "/");
                    strcat(new_dir, commands_list[i] + 1); // Append the argument

                    if (chdir(new_dir) == 0) {
                        strcpy(prev_dir, curr_dir);
                        strcpy(curr_dir, new_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Changed to directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                }

            } else if (commands_list[i][0] == '.' && commands_list[i][1] == '.') {
                char* parent_dir = getparentDir(curr_dir);
                if (strlen(commands_list[i]) == 2) {
                    if (chdir(parent_dir) == 0) {
                        strcpy(prev_dir, curr_dir);
                        strcpy(curr_dir, parent_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Moved up to parent directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                    free(parent_dir);
                } else {
                    char new_dir[4096] = "\0";
                    strcat(new_dir, parent_dir);
                    strcat(new_dir, commands_list[i] + 2); // Append the argument

                    if (chdir(new_dir) == 0) {
                        strcpy(prev_dir, curr_dir);
                        strcpy(curr_dir, new_dir);
                        getcwd(curr_dir, sizeof(curr_dir));
                        printf("Changed to directory: %s\n", curr_dir);
                    } else {
                        // printf("Error: directory doesn't exist\n");
                    }
                    // free(parent_dir);
                }

            } else {
                char new_dir[4096] = "\0";
                strcat(new_dir, commands_list[i]); // Append the argument

                if (chdir(new_dir) == 0) {
                    strcpy(prev_dir, curr_dir);
                    strcpy(curr_dir, new_dir);
                    getcwd(curr_dir, sizeof(curr_dir));
                    printf("Changed to directory: %s\n", curr_dir);
                } else {
                    // printf("Error: directory doesn't exist\n");
                }
            }
        }
    }
    return 0;
}
