//permissions.h

#pragma once

#include "commands.h"

static const char OWNER[] = "own";
static const char READ[] = "read";
static const char WRITE[] = "write";

// Check if a user has permission to execute a specific Git command on a repository
int check_user_permission(const char *user, const char *repository_path, enum GIT command);

// Add a specific Git command permission for a user on a repository
int add_own_permission(const char *user, const char *repository_path);

// List all permissions for a user on a repository
int list_user_permissions(const char *user, const char *repository);
