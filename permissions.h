//permissions.h

#pragma once

#include "commands.h"

// Check if a user has permission to execute a specific Git command on a repository
int check_user_permission(const char *user, const char *repository, enum GIT command);

// Add a specific Git command permission for a user on a repository
int add_user_permission(const char *user, const char *repository, enum GIT command);

// Remove a specific Git command permission for a user on a repository
int remove_user_permission(const char *user, const char *repository, enum GIT command);

// Add all Git command permissions for a user on a repository
int add_all_permissions(const char *user, const char *repository);

// Remove all Git command permissions for a user on a repository
int remove_all_permissions(const char *user, const char *repository);

// List all permissions for a user on a repository
int list_user_permissions(const char *user, const char *repository);
