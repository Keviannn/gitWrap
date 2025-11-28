#!/bin/sh

BASE_DIR="$HOME/repo"

if [ ! -d "$BASE_DIR" ]; then
    echo "Base directory '$BASE_DIR' does not exist. Creating it now."
    mkdir -p "$BASE_DIR"
fi

if [ -z "$REPO_NAME" ]; then
    echo "ERROR: REPO_NAME environment variable is not set."
    exit 1
fi

REPO_PATH="$BASE_DIR/$REPO_NAME"

if [ -d "$REPO_PATH" ]; then
    echo "ERROR: Repository '$REPO_NAME' already exists in $BASE_DIR."
    exit 1
fi

echo "Creating bare repository in: $REPO_PATH"
mkdir "$REPO_PATH"
cd "$REPO_PATH"

git init --bare

echo "Repository '$REPO_NAME' created successfully at $REPO_PATH."
