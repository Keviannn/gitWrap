DEST = ./bin/
TARGET = gitwrap

SRC = main.c commands.c utils.c permissions.c repoManager.c
OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -g

all: $(DEST)$(TARGET)

$(DEST):
	mkdir -p $(DEST)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DEST)$(TARGET): $(OBJ) | $(DEST)
	$(CC) $(CFLAGS) -o $@ $^

re: clean all

clean:
	rm -f $(OBJ) $(DEST)$(TARGET)
