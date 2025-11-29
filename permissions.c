// Implementacion de funciones para gestionar permisos de usuario en un sistema.

#include <stdio.h>
#include "permissions.h"

int check_user_permission(const char *user, const char *repository, enum GIT command) 
{
    //TODO: placeholder
    if(command)
        printf("Checking permissions for %s in %s", user, repository);
    return 1;
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
