#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <regex.h>

void fperror(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "%s: ERROR ", "gitWrap"); // Custom prefix
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

int check_repository_name(const char *name)
{
    // Allow NULL names to pass through (handled elsewhere)
    if (name == NULL)
        return 1;
    
    regex_t regex;
    int ret;

    const char *pattern = "^\'[A-Za-z0-9_-]+/[A-Za-z0-9_-]+\'$";

    if (regcomp(&regex, pattern, REG_EXTENDED))
    {
        fperror("Could not compile regex");
        return 0;
    }

    ret = regexec(&regex, name, 0, NULL, 0);
    regfree(&regex);

    if (!ret)
        return 1;

    else if(ret == REG_NOMATCH)
    {
        fperror("Repository name \"%s\" is not valid", name);
        return 0;
    }

    else
    {
        fperror("Could not check repository name");
        return 0;
    }
}
