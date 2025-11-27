//commands.h

enum GIT {
    GIT_CREATE,
    GIT_NOT_ALLOWED,
    GIT_WELCOME,
    GIT_NO_PERMISSION,
    GIT_PUSH,
    GIT_PULL
};

enum GIT classify_command(char *commad);
