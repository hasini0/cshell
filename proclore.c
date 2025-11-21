#include "headers.h"
#include "proclore.h"

void get_process_info(pid_t pid) {
    char path[1024];
    char buffer[1024];
    char process_status[16] = "";
    char vm_size[16] = "";
    int pgrp = 0, tgid = 0;

    // PID
    printf("pid : %d\n", pid);

    // Process Status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *status_file = fopen(path, "r");
    if (status_file == NULL) {
        perror("Error opening status file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), status_file)) {
        if (strncmp(buffer, "State:", 6) == 0) {
            sscanf(buffer + 7, "%s", process_status);
        }
        if (strncmp(buffer, "VmSize:", 7) == 0) {
            sscanf(buffer + 8, "%s", vm_size);
        }
        if (strncmp(buffer, "Tgid:", 5) == 0) {
            sscanf(buffer + 6, "%d", &tgid);
        }
    }
    fclose(status_file);

    // Process Group ID
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(path, "r");
    if (stat_file == NULL) {
        perror("Error opening stat file");
        return;
    }

    int ppid;
    if (fscanf(stat_file, "%*d %*s %*c %d %d", &ppid, &pgrp) != 2) {
        perror("Error reading stat file");
    }
    fclose(stat_file);

    // Executable Path
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    } else {
        strcpy(buffer, "Unknown");  // If the path couldn't be read
    }

    // Determine if the process is in the foreground or background
    char final_status[16] = "";
    if (strcmp(process_status, "R") == 0) {
        if (tgid == pgrp) {
            strcpy(final_status, "R+");
        } else {
            strcpy(final_status, "R");
        }
    } else if (strcmp(process_status, "S") == 0) {
        if (tgid == pgrp) {
            strcpy(final_status, "S+");
        } else {
            strcpy(final_status, "S");
        }
    } else {
        strcpy(final_status, process_status);
    }

    // Output in the specified format
    printf("Process status : %s\n", final_status);
    printf("Process Group : %d\n", pgrp);
    printf("Virtual memory : %s kB\n", vm_size);
    printf("executable path : %s\n", buffer);
}
