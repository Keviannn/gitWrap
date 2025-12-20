#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "repoManager.h"
#include "permissions.h"
#include "utils.h"

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
    char *base_dir = getenv("HOME");
    if (!base_dir)
    {
        fperror(MSG_ERROR, "HOME environment variable not set.\n");
        return 0;
    }
    
    char *user = getenv("SSH_USER");
    
    fperror(MSG_DEBUG, "Base directory for repositories is %s\n", base_dir);

    char *repo_path = malloc(strlen(base_dir) + strlen("/") + strlen(user) + strlen(repository_name) + 1);
    if (!repo_path)
    {
        fperror(MSG_ERROR, "Could not allocate memory for repository path\n");
        return 0;
    }

    sprintf(repo_path, "%s/%s/%s", base_dir, user, repository_name);

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
    // TODO: placeholder
    if (repository_name)
        return 1;
    else
        return 1;
}
