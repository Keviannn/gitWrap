//utils.h

#pragma once

// Print an error message to stderr with formatted output
void fperror(const char *msg, ...);

// Check if the repository name is valid
int check_repository_name(const char *name);
