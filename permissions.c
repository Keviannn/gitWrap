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

