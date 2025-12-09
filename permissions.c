#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "permissions.h"

#include "commands.h"
#include "utils.h"

#define USERS_FILE "../test.conf"

char* get_repo_name(char *repository_path)
{
    // Copies original path
    char *repository_cp = strdup(repository_path);

    // Checks if the copy was successful
    if (repository_cp == NULL)
    {
        fperror("Could not allocate memory for repository_format");
        free(repository_cp);
        return NULL;
    }

    // Get a pointer to the repository name and remove last quote
    char *repository_temp= strrchr(repository_cp, '/') + 1;
    repository_temp[strlen(repository_temp) - 1] = '\0';

    // Copy only the repository name
    char *repository_name = strdup(repository_temp);

    // Checks if the copy was successful
    if (repository_name == NULL)
    {
        fperror("Could not allocate memory for repository_name");
        free(repository_name);
        return NULL;
    }

    // No need for the original copy anymore
    free(repository_cp);

    // Return a string in the heap that only contains the repository name
    return repository_name;
}

int check_user_permission(const char *user, const char *repository_path, enum GIT command) 
{
    FILE *file = fopen(USERS_FILE, "r");

    if (file == NULL)
    {
        fperror("Could not opnen users file");
        return 0;
    }

    char line[256];

    char *user_format = NULL;

    if(asprintf(&user_format, "[%s]", user) == -1)
    {
        fperror("Could not allocate memory for user_format");
        return 0;
    }

    char *repository_name = get_repo_name((char *)repository_path);

    if (repository_name == NULL)
        return 0;

    int grant = 0;

    // Check every line in the file
    while (fgets(line, sizeof(line), file))
    {
        // Ignore comments and blank lines
        if (line[0] == '#' || line[0] == '\n')
            continue;

        fperror("Checking line: %s", line); // Debug line

        // Get only the line without the newline
        strtok(line, "\n"); 
    
        // If the line is the line with the user we are looking
        if (strcmp(line, user_format) == 0)
        {
            fperror("Found user %s\n", user); // Debug line

            fperror("Checking permissions for repository %s\n", repository_name); // Debug line
            // Get more lines until we find an empty line
            while(fgets(line, sizeof(line), file)[0] != '\n')
            {

                // Ignore comments and blank lines
                if (line[0] == '#' || line[0] == '\n')
                    continue;

                // If the line contains the repository name and the word own in it
                if (strstr(line, repository_name) != NULL && strstr(line, OWNER) != NULL)
                {
                    fperror("User %s is owner of %s\n", user, repository_name); // Debug line
                    // User has all permissions so anything is granted
                    grant = 1;
                    break;
                }

                // If the line contains the repository name and the word read in it
                if (strstr(line, repository_name) != NULL && strstr(line, READ) != NULL)
                {
                    if (command == GIT_PULL)
                    {
                        fperror("User %s has read permission for %s\n", user, repository_name); // Debug line
                        grant = 1;
                        break;
                    }
                }

                // If the line contains the repository name and the word write in it
                if (strstr(line, repository_name) != NULL && strstr(line, WRITE) != NULL)
                {
                    if (command == GIT_PUSH)
                    {
                        fperror("User %s has write permission for %s\n", user, repository_name); // Debug line
                        grant = 1;
                        break;
                    }
                }
            }

            // If we find an empty line before finding the repository we dont have permissions
            if (grant == 0)
                fperror("User %s has no permissions for %s\n", user, repository_name);

            // If we find the user but no permissions stop looking
            break;
        }
    }

    // Free allocated strings
    free(user_format);
    free(repository_name);

    // Close file
    fclose(file);

    return grant;
}

int add_user_permission(const char *user, const char *repository, enum GIT command)
{
    //TODO: placeholder
    if(command)
        printf("Adding permission for %s in %s", user, repository);
    return 1;
}

int remove_user_permission(const char *user, const char *repository, enum GIT command)
{
    //TODO: placeholder
    if(command)
        printf("Removing permission for %s in %s", user, repository);
    return 1;
}

int add_all_permissions(const char *user, const char *repository)
{
    //TODO: placeholder
    printf("Adding all permissions for %s in %s", user, repository);
    return 1;
}

int remove_all_permissions(const char *user, const char *repository)
{
    //TODO: placeholder
    printf("Removing all permissions for %s in %s", user, repository);
    return 1;
}

int list_user_permissions(const char *user, const char *repository)
{
    //TODO: placeholder
    printf("Listing permissions for %s in %s", user, repository);
    return 1;
}
