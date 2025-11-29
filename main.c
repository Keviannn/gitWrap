#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "commands.h"
#include "utils.h"

#define LOCATION "./gitWrap/bin"
#define GIT_CONF "../gitwrap.conf"
#define GIT_SHELL "/usr/bin/git-shell"
#define SHELL "/bin/sh"
#define CREATE_REPOSITORY "../create_repository.sh"

int main() 
{
    char *path;
    asprintf(&path, "%s/%s", getenv("HOME"), LOCATION);

    if(chdir(path) != 0) {
        fperror("Could not change directory to %s", path);
        return 1;
    }

    free(path);

    char *ssh_command = getenv("SSH_ORIGINAL_COMMAND");

    char *argv[4];
    argv[1] = "-c";
    argv[3] = NULL;

    switch (classify_command(ssh_command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", getenv("SSH_USER"));
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            setenv("REPO_NAME", strtok(NULL, " "), 1);

            if(execute_in_shell(SHELL, CREATE_REPOSITORY, argv))
                return 1;
            break;
        case GIT_PUSH:
            if(execute_in_shell(GIT_SHELL, ssh_command, argv))
                return 1;
            break;
        case GIT_PULL:
            if(execute_in_shell(GIT_SHELL, ssh_command, argv))
                return 1;
            break;
        case GIT_NO_PERMISSION:
            fperror("Permission denied for command: %s\n", ssh_command);
            break;
        case GIT_NOT_ALLOWED:
            fperror("Command not allowed: %s\n", ssh_command);
            break;
    }
    return 0;
}
