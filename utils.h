//utils.h

#pragma once

// Print an error message to stderr with formatted output
void fperror(const char *msg, ...);

// Execute a command in a specified shell
int execute_in_shell(char *shell, char *command);
