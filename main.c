#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int check_user_permission(const char *user, const char *command) 
{
    // Placeholder for user permission checking logic
    // In a real implementation, this would check a configuration file or database
    return 1; // Allow all for now
}

int main() 
{
    char *cmd = getenv("SSH_ORIGINAL_COMMAND");

    if (cmd == NULL || strlen(cmd) == 0) 
    {
        printf("Hi %s! You've successfully authenticated!\n", getenv("SSH_USER"));
        printf("But you can't log in with a shell :(\n\n");
        printf("Bye!\n");
        return 1;
    }

    if (strncmp(cmd, "git-", 4) == 0) 
    {
        if (!check_user_permission(getenv("SSH_USER"), cmd)) 
        {
            printf("Permission denied for command: %s\n", cmd);
            return 1;
        }

        else
        {
            char *argv[] = {"/usr/bin/git-shell", "-c", cmd, NULL};
            execv("/usr/bin/git-shell", argv);

            perror("Could not execute git command: execv failed");
            return 1;
        }
    } 
    
    else 
    {
        printf("Command not allowed: %s\n", cmd);
        return 1;
    }

    return 0;
}
