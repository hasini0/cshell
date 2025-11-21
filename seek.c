#include "headers.h"
#include "seek.h"

char finalPrintableResult[256][4096];
char nameToBeSearched[4096];
int iterator = 0;
char finalPathForEFlag[4096];

// name to be searched for is global

void mid_seek(int d_flag, int e_flag, int f_flag, char* searchableDir, char* modifiedSearchableDir) {
    DIR* dir;
    struct dirent* entry;
    struct stat file_stat;
    char differentFullPath[4096];
    
    dir = opendir(searchableDir);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') { // Skip hidden files
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", searchableDir, entry->d_name);
        snprintf(differentFullPath, sizeof(differentFullPath), "%s/%s", modifiedSearchableDir, entry->d_name);

        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {  // Directory
            if (d_flag && strncmp(entry->d_name, nameToBeSearched, strlen(nameToBeSearched)) == 0) {
                strcpy(finalPrintableResult[iterator], differentFullPath);
                iterator++;
            }
            mid_seek(d_flag, e_flag, f_flag, full_path, differentFullPath);  // Recurse into subdirectory
        } else if (S_ISREG(file_stat.st_mode)) {  // Regular file
            if (f_flag && strncmp(entry->d_name, nameToBeSearched, strlen(nameToBeSearched)) == 0) {
                strcpy(finalPrintableResult[iterator], differentFullPath);
                iterator++;
            }
        }
    }
    closedir(dir);
}


void seek(char* command) {
    char rootDirName[4096];
    // Have to parse the string all again
    char* tokens[10];  // Array to store tokens, adjust size as needed
    //strcpy(rootDirName, curr_dir);

    char appendableDirectoryGivenInInput[4096];
    char modifiedSearchableDir[4096];
    strcpy(modifiedSearchableDir, ".");

    int token_count = 0;
    int d_flag = 0;
    int e_flag = 0;
    int f_flag = 0;
    int name_flag = 0;
    int changedDueToFlag = 0;

    char* flags = strtok(command, " ");
    while (flags != NULL) {
        tokens[token_count++] = flags;
        if (strcmp(flags, "-d") == 0) {
            d_flag = 1;
        } else if (strcmp(flags, "-e") == 0) {
            e_flag = 1;
        } else if (strcmp(flags, "-f") == 0) {
            f_flag = 1;
        }
        else if (!(strcmp(flags, "seek") == 0) && name_flag == 0) {
            strcpy(nameToBeSearched, flags);
            name_flag = 1;
        }

        else if (!(strcmp(flags, "seek") == 0) && name_flag == 1) {
            // printf("Hi\n");
            char dup1[4096];
            strcpy(dup1, flags);
            if (dup1[0] == '~') {
                strcpy(rootDirName, home_dir);
                changedDueToFlag = 1;
            }
            if (dup1[0] == '-') {
                strcpy(rootDirName, prev_dir);
                changedDueToFlag = 1;
            }
            if (dup1[0] == '.'  || dup1[0] == '/') {
                strcpy(rootDirName, curr_dir);
                changedDueToFlag = 1;
            }
            else {
                strcpy(rootDirName, curr_dir);
                changedDueToFlag = 1;
                
            }
            strcat(rootDirName, "/");
            strcpy(appendableDirectoryGivenInInput, flags);
            strcat(rootDirName, appendableDirectoryGivenInInput);
        }
        flags = strtok(NULL, " ");
    }

    if (changedDueToFlag != 1) {
        strcpy(rootDirName, curr_dir);
    }

    if (d_flag && f_flag) {
        printf("Cannot use -d and -f together\n");
        return;
    }
    char searchableDir[4096];
    strcpy(searchableDir, rootDirName);
    // printf("The flags are: %d %d %d\n", d_flag, e_flag, f_flag);
    // printf("Path is : %s\n", rootDirName);
    // printf("Name to be searched: %s\n", nameToBeSearched);
    mid_seek(d_flag, e_flag, f_flag, searchableDir, modifiedSearchableDir);

    for (int i = 0; i < iterator; i++) {
        printf("%s\n", finalPrintableResult[i]);
    }
    char finalPathForEFlagWhichCanBeSentToHop[4096];
    strcpy (finalPathForEFlagWhichCanBeSentToHop, finalPrintableResult[0]);



    if (iterator == 1 && e_flag == 1) {
        // printf("The file is found at: %s\n", finalPrintableResult[0]);
        strcpy(finalPathForEFlag, rootDirName);
        strcat(finalPathForEFlag, "/");
        strcat(finalPathForEFlag, finalPrintableResult[0]);
        // printf("The file is found at: %s\n", finalPathForEFlag);
        if (d_flag == 1) {
            // printf("%s\n", finalPathForEFlag);
            char hopCommand[4096];
            strcpy(hopCommand, "hop ");
            strcat(hopCommand, finalPathForEFlag);
            hop(prev_dir, hopCommand);
        }

        if (f_flag == 1) {
            FILE *file = fopen(finalPathForEFlag, "r");
            if (file == NULL) {
                perror("fopen");
                return;
            }

            // printf("Contents of the file:\n");
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                printf("%s", buffer);
            }
            printf("\n");
            fclose(file);
        }
    }
    iterator = 0;
    memset(finalPrintableResult, 0, sizeof(finalPrintableResult));

}