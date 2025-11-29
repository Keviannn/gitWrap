#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

void fperror(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "%s: ERROR ", "gitWrap"); // Custom prefix
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

int execute_in_shell(char *shell_path, char *command)
{
    char *argv[4];
    argv[0] = shell_path;
    argv[1] = "-c";
    argv[2] = command;
    argv[3] = NULL;

    execv(shell_path, argv);
    fperror("Could not execute git command: execv failed");

    return 1;
}
