#include "commands.h"

#include <string.h>
#include <unistd.h>

enum GIT classify_command(char *command)
{
    if (command == NULL || strlen(command) == 0) 
        return GIT_WELCOME;

    if (strcmp(command, "git-receive-pack") == 0) 
        return GIT_PUSH;

    else if (strcmp(command, "git-upload-pack") == 0)
        return GIT_PULL;

    else if (strcmp(strtok(command, " "), "create") == 0)
        return GIT_CREATE;

    else 
        return GIT_NOT_ALLOWED;
}

