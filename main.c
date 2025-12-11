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
    if (change_to_gitwrap_dir() != 0)
        return 1;

    // Parse SSH_ORIGINAL_COMMAND
    char *env = getenv("SSH_ORIGINAL_COMMAND");
    char *ssh_command;
    char *command = NULL;
    char *repository = NULL;

    // Duplicate the environment variable
    if (env == NULL)
        ssh_command = NULL;
    
    else
    {
        ssh_command = strdup(env);
        command = strtok(ssh_command, " ");
        repository = strtok(NULL, " ");
    }

    // If repository name is not valid stop execution
    if (!check_repository_name(repository))
    {
        // Debug lines
        if(DEBUG)
        {
            if (classify_command(command) == GIT_CREATE)
                fperror(MSG_DEBUG, "Repository name is valid for the command %s\n", command);
            else
                return 1;
        }

        else
        {
            if (!(classify_command(command) == GIT_CREATE))
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
            return 1;
        }
    }

    char *user = getenv("SSH_USER");

    // Classify and execute the command
    switch (classify_command(command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", user);
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            fperror(MSG_DEBUG, "Trying to create repository for user %s\n", user);
            
            if(!add_own_permission(user, repository))
            {
                fperror(MSG_ERROR, "No permissions were added\n");
                break;
            }

            // Check if repository name is too long
            if (strlen(repository) > 256)
            {
                fperror(MSG_ERROR, "Repository name too long\n");
                break;
            }

            //TODO: review if create repository could fail in order to rollback permission addition

            //TODO: change script form bash to C
            setenv("REPO_NAME", repository, 1);
            if (execute_in_shell(SHELL, CREATE_REPOSITORY))
                return 1;

            break;
        case GIT_DELETE:
            //TODO: git DELETE implementation
            break;
        case GIT_PUSH:
            if (check_user_permission(user, real_path, GIT_PUSH) != 1) 
            {
                fperror(MSG_ERROR, "Permission denied for command: %s\n", command);
                return 1;
            }
            if (execute_in_shell(GIT_SHELL, final_command))
                return 1;
            break;
        case GIT_PULL:
            // Check if the user has pull permission (for private repositories)
            if (check_user_permission(user, real_path, GIT_PULL) != 1) 
            {
                fperror(MSG_ERROR, "Permission denied for command: %s\n", command);
                return 1;
            }
            if (execute_in_shell(GIT_SHELL, final_command))
                return 1;
            break;
        case GIT_NOT_ALLOWED:
            fperror(MSG_ERROR, "Command not allowed: %s\n", command);
            break;
    }

    if (ssh_command != NULL)
        free(ssh_command);

    free(final_command);
    free(real_path);

    return 0;
}
