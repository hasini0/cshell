#include "headers.h"
#include "hop.h"

char pathInReveal[4096];

// Color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_BLUE    "\033[1;34m"

// Function to print file permissions
void print_permissions(mode_t mode) {
    char permissions[11] = {'-'};
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    if (S_ISDIR(mode)) {
        permissions[0] = 'd';
    } else if (S_ISLNK(mode)) {
        permissions[0] = 'l';
    }

    printf("%s", permissions);
}

// Function to print file details (for -l flag)
void print_file_details(const char *name, struct stat *file_stat) {
    print_permissions(file_stat->st_mode);
    printf(" %ld", (long)file_stat->st_nlink);
    
    // Print owner and group names
    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group *gr = getgrgid(file_stat->st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);
    
    // Print size
    printf(" %ld", (long)file_stat->st_size);
    
    // Print last modified time
    char time_buf[80];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat->st_mtime));
    printf(" %s", time_buf);
    
    // Determine color based on file type
    if (S_ISDIR(file_stat->st_mode)) {
        printf(" " COLOR_BLUE "%s" COLOR_RESET "\n", name);
    } else if (file_stat->st_mode & S_IXUSR) {
        printf(" " COLOR_GREEN "%s" COLOR_RESET "\n", name);
    } else {
        printf(" " COLOR_WHITE "%s" COLOR_RESET "\n", name);
    }
}

// The main ls-like function with -a and -l flag handling
void my_ls(int show_all, int long_format, char* command) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char full_path[4096];  // Buffer to hold the full path
    char fullPathForTilda[4096];
    char fullPathForPreviousDir[4096];
    char fullPathForLastUsedDir[4096];

    // Handle paths starting with "/"
    if (pathInReveal[0] == '/') {
        // Remove the starting slash
        for (int i = 0; i < strlen(pathInReveal); i++) {
            pathInReveal[i] = pathInReveal[i + 1];
        }
        dir = opendir(pathInReveal);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
    }
    // Handle paths starting with "~"
    else if (pathInReveal[0] == '~') {
        if (strlen(pathInReveal) == 1) {
            strcpy(fullPathForTilda, home_dir);
        } else {
            for (int i = 0; i < strlen(pathInReveal); i++) {
                pathInReveal[i] = pathInReveal[i + 1];
            }
            for (int i = 0; i < strlen(pathInReveal); i++) {
                pathInReveal[i] = pathInReveal[i + 1];
            }
            snprintf(fullPathForTilda, sizeof(fullPathForTilda), "%s/%s", home_dir, pathInReveal);
        }
        strcpy(pathInReveal, fullPathForTilda);
        dir = opendir(fullPathForTilda);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
    }
    // Handle paths starting with ".."
    else if (pathInReveal[0] == '.' && pathInReveal[1] == '.') {
        char t[4096];
        strcpy(t, curr_dir);
        char *parentDirJustForProc = getparentDir(t);
        if (strlen(pathInReveal) == 2) {
            dir = opendir(parentDirJustForProc);
            if (dir == NULL) {
                perror("opendir");
                return;
            }
        } else {
            for (int i = 0; i < strlen(pathInReveal); i++) {
                pathInReveal[i] = pathInReveal[i + 1];
            }
            for (int i = 0; i < strlen(pathInReveal); i++) {
                pathInReveal[i] = pathInReveal[i + 1];
            }
            for (int i = 0; i < strlen(pathInReveal); i++) {
                pathInReveal[i] = pathInReveal[i + 1];
            }
            snprintf(fullPathForPreviousDir, sizeof(fullPathForPreviousDir), "%s/%s", parentDirJustForProc, pathInReveal);
            strcpy(pathInReveal, fullPathForPreviousDir);
            dir = opendir(fullPathForPreviousDir);
            
            if (dir == NULL) {
                perror("opendir");
                return;
            }
        }
    }

    // else if (pathInReveal[0] == '-') {
    //     if (strlen(pathInReveal) == 1) {
    //     strcpy(pathInReveal, prev_dir);
    //     dir = opendir(prev_dir);
    //     if (dir == NULL) {
    //         perror("opendir");
    //         return;
    //     }

    //     else {
    //         for (int i = 0; i < strlen(pathInReveal); i++) {
    //             pathInReveal[i] = pathInReveal[i + 1];
    //         }
    //         for (int i = 0; i < strlen(pathInReveal); i++) {
    //             pathInReveal[i] = pathInReveal[i + 1];
    //         }

    //         snprintf(fullPathForLastUsedDir, sizeof(fullPathForLastUsedDir), "%s/%s", prev_dir, pathInReveal);
    //         strcpy(pathInReveal, fullPathForLastUsedDir);
    //         printf("Path in reveal: %s\n", pathInReveal);
    //         dir = opendir(fullPathForLastUsedDir);
    //         if (dir == NULL) {
    //             perror("opendir");
    //             return;
    //         }
    //     }

    //     }
    // }
    else if (pathInReveal[0] == '-') {
    if (strlen(pathInReveal) == 1) {
        // Case 1: Just `-`, resolve to previous directory
        strcpy(pathInReveal, prev_dir);
        dir = opendir(pathInReveal);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
    } else {
        // Case 2: `-` followed by a path
        char pathSuffix[4096];
        // Remove the initial `-`
        strcpy(pathSuffix, pathInReveal + 1);

        // Build the full path relative to prev_dir
        snprintf(fullPathForLastUsedDir, sizeof(fullPathForLastUsedDir), "%s/%s", prev_dir, pathSuffix);
        strcpy(pathInReveal, fullPathForLastUsedDir);
        
        printf("Path in reveal: %s\n", pathInReveal);
        dir = opendir(pathInReveal);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
    }
}

    // Handle relative paths
    else {
        dir = opendir(pathInReveal);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files unless -a flag is set
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        // Construct the full path
        snprintf(full_path, sizeof(full_path), "%s/%s", pathInReveal, entry->d_name);
        // Append full path to full path
        //snprintf(full_path, sizeof(full_path), "%s/%s", fullPathForPreviousDir, full_path);
//       printf("%s\n", full_path);
        if (long_format) {
            // Get file stats
            if (stat(full_path, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            // Print detailed info with color
            print_file_details(entry->d_name, &file_stat);
        } else {
            // Determine color based on file type
            if (stat(full_path, &file_stat) == 0) {
                if (S_ISDIR(file_stat.st_mode)) {
                    printf(COLOR_BLUE "%s" COLOR_RESET "\n", entry->d_name);
                } else if (file_stat.st_mode & S_IXUSR) {
                    printf(COLOR_GREEN "%s" COLOR_RESET "\n", entry->d_name);
                } else {
                    printf(COLOR_WHITE "%s" COLOR_RESET "\n", entry->d_name);
                }
            } else {
                printf("%s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
}

void revealStart(char* command) {
    int show_all = 0;  // -a flag
    int long_format = 0;  // -l flag
    char* tokens[10];  // Array to store tokens, adjust size as needed
    int token_count = 0;
    int iterator = 0; 

    bool hasPathBeenGiven = false;

    // Parse flags
    char* flags = strtok(command, " ");
    while (flags != NULL) {
        tokens[token_count++] = flags;
        //Checking for flags in each string
        if (strcmp(flags, "-a") == 0) {
            show_all = 1;
        } else if (strcmp(flags, "-l") == 0) {
            long_format = 1;
        }
        else if (strcmp(flags, "-al") == 0 || strcmp(flags, "-la") == 0) {
            show_all = 1;
            long_format = 1;
        }
        else {
            if (flags[0] == '-' && strlen(flags) > 1) {
            // Iterate through each character in the flag
            for (int i = 1; i < strlen(flags); i++) {
                if (flags[i] == 'a') {
                    show_all = 1;
                } 
                else if (flags[i] == 'l') {
                    long_format = 1;
                } 
            }
            } 
            else {
                if (iterator > 0 && ((flags[0] == '-' && strlen(flags) == 1) || flags[0] != '-')) {
                    strcpy(pathInReveal, flags);
                    hasPathBeenGiven = true;
                    break;
                }
            }
        }
        flags = strtok(NULL, " ");
        iterator++;
    }

    if (token_count == 1) {
        strcpy(pathInReveal, ".");
    }
    if (!hasPathBeenGiven) {
        strcpy(pathInReveal, ".");
    }

    //printf("Path in reveal: %s\n\n", pathInReveal);
    my_ls(show_all, long_format, command);
}
