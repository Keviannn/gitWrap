# gitWrap - A Git Wrapper for Servers

gitWrap is a lightweight Git wrapper designed for server environments mainly written in C. It manages the creation of new repositories and handles users and permissions on different repositories. It provides a simple and efficient way to host Git repositories.

## Description

I've recently started usign Git a lot more for various projects. I, as most people, use GitHub for hosting my repositories (you are probaly reading this from it right now). 
However, I wanted to have a way to host my own repositories on my own server and I wanted to have an experience near to GitHub but without leaving the command line.

I started from the basics: saying hello to Git users via SSH but don't giving them shell access.

Then I started liking the idea, and here we are now.

## Roadmap

This are the basic starting prerequisites for gitWrap:

- [x] Basic Git over SSH functionality.
- [x] User validation and identificaition.
- [x] Welcome users with a custom message upon SSH connection.
- [x] Remote repository creation.
    - [x] Complete repository creation script.
    - [x] Correctly call the script from the binary.
    - [x] Create user folders automatically.
- [ ] Remote repository deletion.
    - [ ] Complete repository deletion based on user.
    - [ ] Add safety checks to avoid accidental deletions.
- [x] Access to repositories with git@gitWrap:User/repoName syntax.
- [ ] User permission management.
    - [ ] Different permission levels.
    - [x] Automatically set permissions on repository creation.
    - [ ] Automatically remove user access on repository deletion.
- [ ] Change all bash scripts to C functions.

Right now users are expected to modify permissions for other users to have acces to other repositories manually.

More features will be added to the roadmap as the project evolves.

## Installation

To install gitWrap, follow these steps:

1. Get the binary from the releases page or compile it from source.
2. Place the binary in a directory of your choice on your server.
3. Set up SSH Keys for different users in the authorized_keys file of the git user:

    ```bash
    command="SSH_USER=Username /path/to/gitwrap" ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAr...
    command="SSH_USER=Username2 /path/to/gitwrap" ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIB...
    ```
4. Make sure the gitWrap binary is executable.
5. Start using gitWrap to manage your Git repositories.

**Note:** At the actual state of the project, some files and configurations might need to be set up manually. In the future, the binary will handle more of these tasks automatically.

## Usage

To use gitWrap, after setting up SSH keys as described in the installation section, users can interact with their repositories using standard Git commands over SSH.
For example, to clone a repository, a user would run:

```bash
git clone git@gitWrap:Username/repoName
``` 
And for other Git operations like push, pull, etc., the commands remain the same as with any Git repository.

---

For repository creation, users can use a command like:

```bash
ssh git@gitWrap create repoName
```

---

**Permissions and user management is still under development.**

## Curiosities
- This project is being tracked in my own server, and it is configured so every time I push something to my repository, it is compiled with a hook, so it is always ready to test.
- The project was born from the need to have a server for an university social media group project, so I decided to host it myself, as I already had a server running for other purposes, but still wanted everyone to be able to contribute easily.

