#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <fcntl.h>

#include "display.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"

extern char home_dir[4096];
extern char prev_dir[4096];
extern char curr_dir[4096];
extern char* commands[4096];
extern char absoluteHomeDir[4096];

typedef struct {
    char *command;
    int is_background;
} Command;

#endif