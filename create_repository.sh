#!/bin/sh

#TODO: hacerlo más seguro y completo, asegurar que todo existe para instalaciones de 0

cd $HOME/repo
mkdir $REPO_NAME
cd $REPO_NAME
git init --bare
