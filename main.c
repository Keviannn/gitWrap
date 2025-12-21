#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "commands.h"
#include "utils.h"
#include "permissions.h"
#include "repoManager.h"

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
    fperror(MSG_ERROR, "Could not execute git command: execv failed");

    return 1;
}

// Fix the repository path by adding '~' at the beginning
char *fix_path(const char *path)
{
    char *fixed = NULL;

    if (path != NULL)
    {
        if (path[0] == '\'')
            path++;

        if (asprintf(&fixed, "%s/%s", "\'~", path) == -1)
        {
            fperror(MSG_ERROR, "Could not allocate memory for fixed");
            return NULL;
        }
    }

    return fixed;
}

// Change to gitWrap directory
int change_to_gitwrap_dir()
{
    char *path;

    if (asprintf(&path, "%s/%s", getenv("HOME"), LOCATION) == -1) 
    {
        fperror(MSG_ERROR, "Memory allocation error");
        return 1;
    }

    if (chdir(path) != 0)
    {
        fperror(MSG_ERROR, "Could not change directory to %s", path);
        return 1;
    }

    free(path);

    return 0;
}

int main() 
{
    fperror(MSG_DEBUG, "Starting gitWrap...\n");

    if (change_to_gitwrap_dir() != 0)
        return 1;

    char *user = getenv("SSH_USER");

    // Parse SSH_ORIGINAL_COMMAND
    char *env = getenv("SSH_ORIGINAL_COMMAND");
    char *ssh_command;
    char *command = NULL;
    char *repository = NULL;

    fperror(MSG_DEBUG, "SSH_ORIGINAL_COMMAND: %s\n", env);

    // Duplicate the environment variable
    if (env == NULL)
        ssh_command = NULL;
    else
    {
        ssh_command = strdup(env);
        command = strtok(ssh_command, " ");
        repository = strtok(NULL, " ");
    }

    fperror(MSG_DEBUG, "Command: %s\n", command);
    fperror(MSG_DEBUG, "Repository: %s\n", repository); 

    // If repository name is not valid...
    if (!check_repository_name(repository))
    {
        // Debug lines
        if(DEBUG)
        {
            if (classify_command(command) == GIT_CREATE)
                fperror(MSG_DEBUG, "Repository name is valid for the command %s\n", command);
        }

        // And if the command is not create or delete, exit
        if (classify_command(command) != GIT_CREATE && classify_command(command) != GIT_DELETE)
        {
            free(ssh_command);
            return 1;
        }
    }


    char *final_command = NULL;
    char *real_path = NULL;

    // Fix the repository path if command is not create
    if(classify_command(command) != GIT_CREATE)
    {
        real_path = fix_path(repository);
        
        // Create the final command to execute
        if (asprintf(&final_command, "%s %s", command, real_path) == -1)
        {
            fperror(MSG_ERROR, "Could not allocate memory for final_command");
            free(real_path);
            return 1;
        }
    }

    fperror(MSG_DEBUG, "Final command to execute: %s\n", final_command);
    fperror(MSG_DEBUG, "Real path: %s\n", real_path);

    if(DEBUG)
        list_user_permissions(user);

    // Classify and execute the command
    switch (classify_command(command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", user);
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            fperror(MSG_DEBUG, "Trying to create repository %s for user %s\n", repository, user);

            // Check if repository name is too long
            if (strlen(repository) > 200)
            {
                fperror(MSG_ERROR, "Repository name must be less than 200 characters\n");
                break;
            }

            create_repository(repository);

            break;
        case GIT_DELETE:
            fperror(MSG_DEBUG, "Trying to delete repository %s for user %s\n", repository, user);

            // Check if the repository name is too long
            if (strlen(repository) > 200)
            {
                fperror(MSG_ERROR, "Repository name must be less than 200 characters\n");
                break;
            }

            delete_repository(repository);

            break;
        case GIT_PUSH:
            // Check if the user has push permission
            if (!check_user_permission(user, real_path, GIT_PUSH)) 
            {
                fperror(MSG_ERROR, "Permission denied for command: %s\n", command);
                break;
            }

            if (execute_in_shell(GIT_SHELL, final_command))
                fperror(MSG_ERROR, "Failed to execute command: %s\n", command);

            break;
        case GIT_PULL:
            // Check if the user has pull permission (for private repositories)
            if (!check_user_permission(user, real_path, GIT_PULL)) 
            {
                fperror(MSG_ERROR, "Permission denied for command: %s\n", command);
                break;
            }
            if (execute_in_shell(GIT_SHELL, final_command))
                fperror(MSG_ERROR, "Failed to execute command: %s\n", command);
            break;
        case GIT_NOT_ALLOWED:
            fperror(MSG_ERROR, "Command not allowed: %s\n", command);
            break;
    }

    free(ssh_command);
    free(real_path);
    free(final_command);

    return 0;
}
