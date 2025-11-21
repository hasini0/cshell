#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H

#include <sys/types.h>

extern pid_t bg_processes[];
extern int bg_process_count;

void check_background_processes();
void handle_fork_and_execute(char* command, int is_background);
void sigchld_handler(int signum);
void setup_sigchld_handler();

#endif
