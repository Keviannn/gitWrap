#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int check_user_permission(const char *user, enum GIT command) 
{
    //TODO: placeholder
    if(command)
        printf("Checking permissions for: %s", user);
    return 1;
}

enum GIT classify_command(char *command)
{
    if (command == NULL || strlen(command) == 0) 
        return GIT_WELCOME;

    if (strcmp(command, "git-receive-pack") == 0) 
        if (!check_user_permission(getenv("SSH_USER"), GIT_PUSH)) 
            return GIT_NO_PERMISSION;
        else
            return GIT_PUSH;

    else if (strcmp(command, "git-upload-pack") == 0)
        return GIT_PULL;

    else if (strncmp(strtok(command, " "), "create", 6) == 0)
        return GIT_CREATE;

    else 
        return GIT_NOT_ALLOWED;
}

