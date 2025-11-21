#include "headers.h"

void execute_command(Command commands[], Command commands2[], int i, char* prev_dir, bool isTheNewCommandTheSameAsTheOldOne);
void run_command(Command commands[], Command commands2[], int i, char* prev_dir);
int split_string(char *str, Command commands[], Command commands2[], int max_commands, int* doesLogExistInTheCommand);
int execute_with_timing(Command commands[], Command commands2[], int i, char* prev_dir, bool isTheNewCommandTheSameAsTheOldOne);
