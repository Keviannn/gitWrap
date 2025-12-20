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

// Convert permission string to enum
enum PERMISSIONS permission_from_string(const char *permission_str)
{
    if (strstr(permission_str, "own") != NULL)
        return PERMISSION_OWNER;
    else if (strstr(permission_str, "colaborator") != NULL)
        return PERMISSION_COLABORATOR;
    else if (strstr(permission_str, "read") != NULL)
        return PERMISSION_READ;
    else if (strstr(permission_str, "write") != NULL)
        return PERMISSION_WRITE;
    else
        return PERMISSION_NONE;
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

    char found_user = 0;
    char found_repository = 0;
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
            found_user = 1;

            fperror(MSG_DEBUG, "Found user %s\n", user);

            fperror(MSG_DEBUG, "Checking permissions for repository %s\n", repository_name);

            // Get more lines after we find the user
            while(fgets(line, sizeof(line), file))
            {
                // Ignore comments and blank lines
                if (line[0] == '#' || line[0] == '\n')
                    continue;

                // Get only the line without the newline
                strtok(line, "\n"); 

                // If the line contains the repository name
                if (strstr(line, repository_name) != NULL)
                {
                    fperror(MSG_DEBUG, "Found repository %s on line: %s\n", repository_name, line);

                    found_repository = 1;

                    // If the line defines owner permission
                    if (permission_from_string(line) == PERMISSION_OWNER)
                    {
                        fperror(MSG_DEBUG, "User %s is owner of %s\n", user, repository_name);
                        // User has all permissions so anything is granted
                        grant = 1;
                        break;
                    }

                    // If the line contains contains colaborator permission
                    if (permission_from_string(line) == PERMISSION_COLABORATOR)
                    {
                        fperror(MSG_DEBUG, "User %s is colaborator of %s\n", user, repository_name);
                        // Colaborators have all permissions so anything is granted
                        grant = 1;
                        break;
                    }

                    // If the line contains read permission
                    if (permission_from_string(line) == PERMISSION_READ)
                    {
                        if (command == GIT_PULL)
                        {
                            fperror(MSG_DEBUG, "User %s has read permission for %s\n", user, repository_name);
                            grant = 1;
                            break;
                        }
                    }

                    // If the line contains write permission
                    if (permission_from_string(line) == PERMISSION_WRITE)
                    {
                        if (command == GIT_PUSH)
                        {
                            fperror(MSG_DEBUG, "User %s has write permission for %s\n", user, repository_name);
                            grant = 1;
                            break;
                        }
                    }
                    
                    // If, by any chance, there are no matching permissions
                    if (permission_from_string(line) == PERMISSION_NONE)
                    {
                        fperror(MSG_ERROR, "User %s has a non valid permission tag for %s\n", user, repository_name);
                        break;
                    }
                }

            }
            
            // If we find no repository under the user
            if (found_repository == 0)
            {
                fperror(MSG_ERROR, "Repository %s not found under user %s\n", repository_name, user);
                break;
            }

        }
        
        // If we finish reading lines and didnt find the user
        if (found_user == 0)
        {
            fperror(MSG_ERROR, "User %s not found in users file\n", user);
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

// Add own permission for a user and repository
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

// Remove permission for a user and repository
int remove_permission(const char *user, const char *repository_name)
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

    int removed = 0;

    // Check every line in the file
    while (fgets(line, sizeof(line), file))
    {
        // Get only the line without the newline
        strtok(line, "\n"); 

        // If the line is the line with the user
        if (strcmp(line, user_format) == 0)
        {
            fperror(MSG_DEBUG, "Found user %s\n", user);

            // Get more lines after we find the user
            while(fgets(line, sizeof(line), file))
            { 
                // If we find the repository name, skip writing this line to remove it
                if(strstr(line, repository_name))
                {
                    fperror(MSG_DEBUG, "Removing permissions for %s under the user %s\n", repository_name, user);
                    removed = 1;
                    fgets(line, sizeof(line), file); // Read next line for the next write
                    break;
                }
            }
        }

        // Write the line to the temporary file
        fwrite(line, sizeof(char), strlen(line), temp_file);
    }

    // Free allocated strings
    free(user_format);

    // Close files
    fclose(file);
    fclose(temp_file);

    // Replace original file with temporary file
    remove(USERS_FILE);
    rename("temp.conf", USERS_FILE);

    return removed;
}

// Show user permissions
void list_user_permissions(const char *user)
{
    FILE *file = fopen(USERS_FILE, "r");

    if (file == NULL)
    {
        fperror(MSG_ERROR, "Could not open users file");
        return;
    }

    char line[256];

    char *user_format = NULL;

    if(asprintf(&user_format, "[%s]", user) == -1)
    {
        fperror(MSG_ERROR, "Could not allocate memory for user_format");
        return;
    }

    // Check every line in the file
    while (fgets(line, sizeof(line), file))
    {
        // Get only the line without the newline
        strtok(line, "\n"); 

        // If the line is the line with the user we are looking for
        if (strcmp(line, user_format) == 0)
        {
            fperror(MSG_DEBUG, "Listing permissions for user %s\n", user);

            // Get more lines after we find the user
            while(fgets(line, sizeof(line), file))
            {
                // Ignore comments and blank lines
                if (line[0] == '#' || line[0] == '\n')
                    continue;

                // Print lines until we reach another user section
                if (line[0] == '[')
                    break;

                // Print the permission line
                fperror(MSG_DEBUG, "Permission line: %s", line);
            }
        }
    }

    // Free allocated strings
    free(user_format);

    // Close files
    fclose(file);
}
