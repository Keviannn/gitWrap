//utils.h

#pragma once

void fperror(const char *msg, ...);
int execute_in_shell(char *shell_path, char *command, char *argv[4]);
