DEST = ./bin/
TARGET = gitwrap

SRC = main.c commands.c utils.c permissions.c
OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3

all: $(DEST)$(TARGET)

$(DEST):
	mkdir -p $(DEST)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DEST)$(TARGET): $(OBJ) | $(DEST)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(DEST)$(TARGET)
