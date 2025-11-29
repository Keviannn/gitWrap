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
