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

// Execute a command in the specified shell
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

// Fix the repository path by adding '~' at the beginning
char *fix_path(const char *path)
{
    char *fixed = NULL;

    if(path != NULL)
    {
        if(path[0] == '\'')
            path++;

        if(asprintf(&fixed, "%s/%s", "\'~", path) == -1)
        {
            fperror("Could not allocate memory for fixed");
            return NULL;
        }
    }

    return fixed;
}

// Change to gitWrap directory
int change_to_gitwrap_dir()
{
    char *path;

    if(asprintf(&path, "%s/%s", getenv("HOME"), LOCATION) == -1) {
        fperror("Memory allocation error");
        return 1;
    }

    if(chdir(path) != 0) {
        fperror("Could not change directory to %s", path);
        return 1;
    }

    free(path);

    return 0;
}

int main() 
{
    if(change_to_gitwrap_dir() != 0)
        return 1;

    // Parse SSH_ORIGINAL_COMMAND
    char *env = getenv("SSH_ORIGINAL_COMMAND");
    char *ssh_command;
    char *command = NULL;
    char *repository = NULL;

    if(env == NULL)
        ssh_command = NULL;
    else
    {
        ssh_command = strdup(env);
        // Tokenize the command
        command = strtok(ssh_command, " ");
        repository = strtok(NULL, " ");
    }

    // Fix the repository path
    char *real_path = fix_path(repository);
    char *final_command = NULL;

    // Check for memory allocation error
    if(asprintf(&final_command, "%s %s", command, real_path) == -1)
    {
        fperror("Could not allocate memory for final_command");
        return 1;
    }

    // Classify and execute the command
    switch (classify_command(command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", getenv("SSH_USER"));
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            setenv("REPO_NAME", repository, 1);

            add_all_permissions(getenv("SSH_USER"), repository);

            if(execute_in_shell(SHELL, CREATE_REPOSITORY))
            {
                remove_all_permissions(getenv("SSH_USER"), real_path);
                return 1;
            }
            break;
        case GIT_PUSH:
            if(check_user_permission(getenv("SSH_USER"), real_path, GIT_PUSH) != 1) 
            {
                fperror("Permission denied for command: %s\n", command);
                return 1;
            }
            if(execute_in_shell(GIT_SHELL, final_command))
                return 1;
            break;
        case GIT_PULL:
            // Check if the user has pull permission (for private repositories)
            if(check_user_permission(getenv("SSH_USER"), real_path, GIT_PULL) != 1) 
            {
                fperror("Permission denied for command: %s\n", command);
                return 1;
            }
            if(execute_in_shell(GIT_SHELL, final_command))
                return 1;
            break;
        case GIT_NOT_ALLOWED:
            fperror("Command not allowed: %s\n", command);
            break;
    }

    if(ssh_command != NULL)
        free(ssh_command);

    free(final_command);
    free(real_path);

    return 0;
}
