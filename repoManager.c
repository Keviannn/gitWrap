#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "repoManager.h"
#include "permissions.h"
#include "utils.h"

// Get the full path for the repository
char *get_full_path(const char *repository_name, const char *user)
{
    char *base_dir = getenv("HOME");
    if (!base_dir)
    {
        fperror(MSG_ERROR, "HOME environment variable not set.\n");
        return NULL;
    }
    
    fperror(MSG_DEBUG, "Base directory for repositories is %s\n", base_dir);

    char *repo_path = malloc(strlen(base_dir) + 1 + strlen(user) + 1 + strlen(repository_name) + 1);
    if (!repo_path)
    {
        fperror(MSG_ERROR, "Could not allocate memory for repository path\n");
        return NULL;
    }

    sprintf(repo_path, "%s/%s/%s", base_dir, user, repository_name);

    fperror(MSG_DEBUG, "Path for new repository is %s\n", repo_path);

    return repo_path;
}

// Check if a directory exists
int directory_exists(const char *path)
{
    if (path == NULL)
        return 0;

    DIR *dir = opendir(path);
    if (dir)
    {
        closedir(dir);
        return 1;
    }

    return 0;
}

// Create a new repository with the given name
int create_repository(const char *repository_name)
{
    char *user = getenv("SSH_USER");
    if (user == NULL)
    {
        fperror(MSG_ERROR, "Could not get SSH_USER environment variable.\n");
        return 0;
    }

    char *repo_path = get_full_path(repository_name, user);
    if (repo_path == NULL)
    {
        fperror(MSG_ERROR, "Could not get full path for repository %s.\n", repository_name);
        return 0;
    }

    fperror(MSG_DEBUG, "Path for new repository is %s\n", repo_path);

    // Check if the repository already exists
    if (directory_exists(repo_path)) 
    {
        fperror(MSG_ERROR, "Repository '%s' already exists for %s.\n", repository_name, user);
        free(repo_path);
        return 0;
    }

    // Create the repository directory
    if (mkdir(repo_path, 0755) != 0)
    {
        fperror(MSG_ERROR, "Could not create repository directory '%s'.\n", repository_name);
        free(repo_path);
        return 0;
    }

    // Initialize a bare git repository
    char command[256];
    snprintf(command, sizeof(command), "cd %s && git init --bare", repo_path);
    int ret = system(command);
    if (ret != 0)
    {
        fperror(MSG_ERROR, "git init failed for repository '%s'.\n", repository_name);
        free(repo_path);
        return 0;
    }

    if (!add_own_permission(user, repository_name))
    {
        // TODO: Cleanup created repository
        free(repo_path);
        return 0;
    }
    
    free(repo_path);
    return 1;
}

// Delete the repository with the given name
int delete_repository(const char *repository_name)
{
    char *user = getenv("SSH_USER");
    if (user == NULL)
    {
        fperror(MSG_ERROR, "Could not get SSH_USER environment variable.\n");
        return 0;
    }

    char *repository_path = get_full_path(repository_name, user);
    if (repository_path == NULL)
    {
        fperror(MSG_ERROR, "Could not get full path for repository %s.\n", repository_name);
        return 0;
    }

    if (!check_user_permission(user, repository_path, GIT_DELETE))
    {
        fperror(MSG_ERROR, "User %s does not have permission to delete repository %s.\n", user, repository_name);
        return 0;
    }
    
    if (!directory_exists(repository_path))
    {
        fperror(MSG_ERROR, "Repository %s does not exist.\n", repository_name);
        return 0;
    }

    fperror(MSG_INFO, "Trying to delete repository %s, are you sure? Y/N\n", repository_name);
    char c = getchar();
    if (c != 'Y' && c != 'y')
    {
        fperror(MSG_INFO, "Aborting deletion of repository %s.\n", repository_name);
        return 0;
    }

    fperror(MSG_INFO, "Input the repository name again to confirm deletion: ");
    char confirm_name[201];
    if (scanf("%200s", confirm_name) != 1)
    {
        fperror(MSG_ERROR, "Failed to read confirmation input.\n");
        return 0;
    }

    if (strcmp(confirm_name, repository_name) != 0)
    {
        fperror(MSG_ERROR, "Repository name does not match. Aborting deletion.\n");
        return 0;
    }

    // Remove the repository directory
    char *command = malloc(strlen(repository_path) + 10);
    if (command == NULL)
    {
        fperror(MSG_ERROR, "Memory allocation failed.\n");
        return 0;
    }   

    sprintf(command, "rm -rf %s", repository_path);
    int ret = system(command);
    free(command);
    if (ret != 0)
    {
        fperror(MSG_ERROR, "Could not delete repository '%s'.\n", repository_name);
        return 0;
    }

    fperror(MSG_INFO, "Repository %s deleted successfully.\n", repository_name);

    remove_permission(user, repository_name);

    return 1;
}
