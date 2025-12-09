//commands.h

#pragma once

// Enumeration of Git command types
enum GIT 
{
    GIT_CREATE,
    GIT_NOT_ALLOWED,
    GIT_WELCOME,
    GIT_PUSH,
    GIT_PULL
};

// Classify the given command and return the corresponding GIT enum value
enum GIT classify_command(char *commad);
