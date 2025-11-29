#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "commands.h"
#include "utils.h"
#include "permissions.h"

#define LOCATION "./gitWrap/bin"
#define GIT_CONF "../gitwrap.conf"
#define GIT_SHELL "/usr/bin/git-shell"
#define SHELL "/bin/sh"
#define CREATE_REPOSITORY "../create_repository.sh"

int main() 
{
    char *path;

    // Change to the gitWrap/bin directory
    if(asprintf(&path, "%s/%s", getenv("HOME"), LOCATION) == -1) {
        fperror("Memory allocation error");
        return 1;
    }

    if(chdir(path) != 0) {
        fperror("Could not change directory to %s", path);
        return 1;
    }

    free(path);

    // Get the original SSH command
    char *ssh_command = getenv("SSH_ORIGINAL_COMMAND");

    // Parse the command to get the repository name
    char *ssh_command_copy = strdup(ssh_command);
    char *command = strtok(ssh_command_copy, " ");
    char *repository = strtok(NULL, " ");

    // Classify and execute the command
    switch (classify_command(command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", getenv("SSH_USER"));
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            setenv("REPO_NAME", strtok(NULL, " "), 1);

            add_all_permissions(getenv("SSH_USER"), repository);

            if(execute_in_shell(SHELL, CREATE_REPOSITORY))
            {
                remove_all_permissions(getenv("SSH_USER"), repository);
                return 1;
            }
            break;
        case GIT_PUSH:
            if(check_user_permission(getenv("SSH_USER"), repository, GIT_PUSH) != 1) {
                fperror("Permission denied for command: %s\n", command);
                return 1;
            }
            if(execute_in_shell(GIT_SHELL, ssh_command))
                return 1;
            break;
        case GIT_PULL:
            // Check if the user has pull permission (for private repositories)
            if(check_user_permission(getenv("SSH_USER"), repository, GIT_PULL) != 1) {
                fperror("Permission denied for command: %s\n", command);
                return 1;
            }
            if(execute_in_shell(GIT_SHELL, ssh_command))
                return 1;
            break;
        case GIT_NOT_ALLOWED:
            fperror("Command not allowed: %s\n", command);
            break;
    }

    free(ssh_command_copy);

    return 0;
}
