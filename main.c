#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"

#define GIT_CONF "./gitwrap.conf"
#define GIT_SHELL "/usr/bin/git-shell"
#define SHELL "/bin/sh"
#define CREATE_REPOSITORY "./create_repository.sh"

int execute_in_shell(char *shell_path, char *command, char *argv[4])
{
    argv[0] = shell_path;
    argv[2] = command;

    execv(shell_path, argv);
    perror("Could not execute git command: execv failed");

    return 1;
}

int main() 
{
    char *ssh_command = getenv("SSH_ORIGINAL_COMMAND");

    char *argv[4];
    argv[1] = "-c";
    argv[3] = NULL;

    char *repo_var;

    switch (classify_command(ssh_command)) {
        case GIT_WELCOME:
            printf("Hi %s! You've successfully authenticated!\n", getenv("SSH_USER"));
            printf("But you can't log in with a shell :(\n\n");
            printf("Bye!\n");
            break;
        case GIT_CREATE:
            asprintf(&repo_var, "REPO_NAME=%s", strtok(NULL, " "));
            argv[0] = SHELL;
            argv[2] = CREATE_REPOSITORY;

            execve(SHELL, argv, &repo_var);
            perror("Could not execute git command: execv failed");

            return 1;
        case GIT_PUSH:
            if(execute_in_shell(GIT_SHELL, ssh_command, argv))
                return 1;
            break;
        case GIT_PULL:
            if(execute_in_shell(GIT_SHELL, ssh_command, argv))
                return 1;
            break;
        case GIT_NO_PERMISSION:
            printf("Permission denied for command: %s\n", ssh_command);
            break;
        case GIT_NOT_ALLOWED:
            printf("Command not allowed: %s\n", ssh_command);
            break;
    }

    return 0;
}
