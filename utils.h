//utils.h

#pragma once

#define DEBUG 1

enum MSG_TYPE {
    MSG_INFO,
    MSG_WARNING,
    MSG_ERROR,
    MSG_DEBUG
};

// Print an error message to stderr with formatted output
void fperror(enum MSG_TYPE type, const char *msg, ...);

// Check if the repository name is valid
int check_repository_name(const char *name);
