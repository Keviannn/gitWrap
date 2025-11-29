#!/bin/sh

# set -e: Exit immediately if a command exits with a non-zero status.
# set -u: Treat unset variables as an error when substituting.
set -eu

NAME=repoMaker

BASE_DIR="$HOME/repo"

mkdir -p "$BASE_DIR" || { echo "$NAME: ERROR Could not create base directory"; exit 1; }

# Check if REPO_NAME is set
if [ -z "$REPO_NAME" ]; then
    echo "$NAME: ERROR REPO_NAME environment variable is not set."
    exit 1
fi

REPO_PATH="$BASE_DIR/$REPO_NAME"

# Check if the repository already exists
if [ -d "$REPO_PATH" ]; then
    echo "$NAME: ERROR Repository '$REPO_NAME' already exists in $BASE_DIR."
    exit 1
fi

echo "Creating bare repository in: $REPO_PATH"
mkdir "$REPO_PATH" || { echo "$NAME: ERROR Could not create repository directory"; exit 1; }

cd "$REPO_PATH" || { echo "$NAME: ERROR Could not change directory to repository path"; exit 1; }
git init --bare || { echo "$NAME: ERROR git init failed"; exit 1; }

echo "Repository '$REPO_NAME' created successfully at $REPO_PATH."
