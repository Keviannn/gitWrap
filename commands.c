#include "commands.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

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

    if (strncmp(command, "git-receive-pack", 16) == 0) 
        if (!check_user_permission(getenv("SSH_USER"), GIT_PUSH)) 
            return GIT_NO_PERMISSION;
        else
            return GIT_PUSH;

    else if (strncmp(command, "git-upload-pack", 15) == 0)
        return GIT_PULL;

    else if (strcmp(strtok(command, " "), "create") == 0)
        return GIT_CREATE;

    else 
        return GIT_NOT_ALLOWED;
}

