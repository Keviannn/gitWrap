#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "permissions.h"

#include "commands.h"
#include "utils.h"

#define USERS_FILE "../users.conf"

// Extracts the repository name from the full repository path
char* get_repo_name(char *repository_path)
{
    // Copies original path
    char *repository_cp = strdup(repository_path);

    // Checks if the copy was successful
    if (repository_cp == NULL)
    {
        fperror(MSG_ERROR, "Could not allocate memory for repository_format\n");
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
        fperror(MSG_ERROR, "Could not allocate memory for repository_name\n");
        free(repository_name);
        return NULL;
    }

    // No need for the original copy anymore
    free(repository_cp);

    // Return a string in the heap that only contains the repository name
    return repository_name;
}

// Check if a user has permission to execute a specific Git command on a repository
int check_user_permission(const char *user, const char *repository_path, enum GIT command) 
{
    FILE *file = fopen(USERS_FILE, "r");

    if (file == NULL)
    {
        fperror(MSG_ERROR, "Could not open users file\n");
        return 0;
    }

    char line[256];

    char *user_format = NULL;

    if(asprintf(&user_format, "[%s]", user) == -1)
    {
        fperror(MSG_ERROR, "Could not allocate memory for user_format\n");
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

        // Get only the line without the newline
        strtok(line, "\n"); 
    
        // If the line is the line with the user we are looking
        if (strcmp(line, user_format) == 0)
        {
            fperror(MSG_DEBUG, "Found user %s\n", user);

            fperror(MSG_DEBUG, "Checking permissions for repository %s\n", repository_name);
            // Get more lines until we find an empty line
            while(fgets(line, sizeof(line), file)[0] != '\n')
            {

                // Ignore comments and blank lines
                if (line[0] == '#' || line[0] == '\n')
                    continue;

                // If the line contains the repository name and the word own in it
                if (strstr(line, repository_name) != NULL && strstr(line, OWNER) != NULL)
                {
                    fperror(MSG_DEBUG, "User %s is owner of %s\n", user, repository_name);
                    // User has all permissions so anything is granted
                    grant = 1;
                    break;
                }

                // If the line contains the repository name and the word read in it
                if (strstr(line, repository_name) != NULL && strstr(line, READ) != NULL)
                {
                    if (command == GIT_PULL)
                    {
                        fperror(MSG_DEBUG, "User %s has read permission for %s\n", user, repository_name);
                        grant = 1;
                        break;
                    }
                }

                // If the line contains the repository name and the word write in it
                if (strstr(line, repository_name) != NULL && strstr(line, WRITE) != NULL)
                {
                    if (command == GIT_PUSH)
                    {
                        fperror(MSG_DEBUG, "User %s has write permission for %s\n", user, repository_name);
                        grant = 1;
                        break;
                    }
                }
            }

            // If we find an empty line before finding the repository we dont have permissions
            if (grant == 0)
                fperror(MSG_ERROR, "User %s has no permissions for %s\n", user, repository_name);

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

int add_own_permission(const char *user, const char *repository_name)
{
    FILE *file = fopen(USERS_FILE, "r+");
    FILE *temp_file = fopen("temp.conf", "w");

    if (file == NULL || temp_file == NULL)
    {
        fperror(MSG_ERROR, "Could not open users file");
        return 0;
    }

    char line[256];

    char *user_format = NULL;

    if(asprintf(&user_format, "[%s]", user) == -1)
    {
        fperror(MSG_ERROR, "Could not allocate memory for user_format");
        return 0;
    }

    int added = 0;
    int found = 0;

    // Check every line in the file
    while (fgets(line, sizeof(line), file))
    {
        // Write the line to the temporary file
        fwrite(line, sizeof(char), strlen(line), temp_file);

        // Get only the line without the newline
        strtok(line, "\n"); 
    
        // If the line is the line with the user we are looking
        if (strcmp(line, user_format) == 0)
        {
            fperror(MSG_DEBUG, "Found user %s\n", user);
            // Get more lines until we find an empty line
            while(fgets(line, sizeof(line), file)[0] != '\n')
            {
                // Write the line to the temporary file
                fwrite(line, sizeof(char), strlen(line), temp_file);

                // Ignore comments
                if (line[0] == '#')
                    continue;

                // If the line contains the repository name
                if (strstr(line, repository_name))
                {
                    fperror(MSG_ERROR, "There are already permissions for %s under the user %s\n", repository_name, user);
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                // If we find an empty line we add permissions
                fperror(MSG_DEBUG, "Adding permissions for %s under the user %s\n", repository_name, user);
                char *perm_line = NULL;
                asprintf(&perm_line, "%s = %s\n\n", repository_name, "own");
                fperror(MSG_DEBUG, "Permission line to add: %s\n", perm_line);
                fwrite(perm_line, sizeof(char), strlen(perm_line), temp_file);
                free(perm_line);
                added = 1;
            }
        }
    }

    // Free allocated strings
    free(user_format);

    // Close files
    fclose(file);
    fclose(temp_file);

    // Replace original file with temporary file
    remove(USERS_FILE);
    rename("temp.conf", USERS_FILE);

    return added;
}

// TODO: implement for fail to create repositories cases
int remove_entries(const char *repository_name)
{
    // Placeholder
    if (repository_name == NULL)
        return 1;
    return 1;
}

int list_user_permissions(const char *user, const char *repository)
{
    //TODO: placeholder
    printf("Listing permissions for %s in %s", user, repository);
    return 1;
}
