#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <regex.h>

void fperror(enum MSG_TYPE type, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    switch(type)
    {
        case MSG_INFO:
            fprintf(stderr, "%s: INFO ", "gitWrap");
            vfprintf(stderr, msg, args);
            break;
        case MSG_WARNING:
            fprintf(stderr, "%s: WARNING ", "gitWrap");
            vfprintf(stderr, msg, args);
            break;
        case MSG_ERROR:
            fprintf(stderr, "%s: ERROR ", "gitWrap");
            vfprintf(stderr, msg, args);
            break;
        case MSG_DEBUG:
            if (DEBUG)
            {
                fprintf(stderr, "%s: DEBUG ", "gitWrap");
                vfprintf(stderr, msg, args);
            }
            break;
    }

    va_end(args);
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
        fperror(MSG_ERROR, "Could not compile regex");
        return 0;
    }

    ret = regexec(&regex, name, 0, NULL, 0);
    regfree(&regex);

    if (!ret)
        return 1;

    else if(ret == REG_NOMATCH)
    {
        fperror(MSG_DEBUG, "Repository name %s is only valid for create command\n", name);
        return 0;
    }

    else
    {
        fperror(MSG_ERROR, "Could not check repository name\n");
        return 0;
    }
}
