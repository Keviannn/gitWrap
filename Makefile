DEST = ./bin/
TARGET = gitwrap

SRC = main.c

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3

all: $(DEST)$(TARGET)

$(DEST):
	mkdir -p $(DEST)

$(DEST)$(TARGET): $(SRC) | $(DEST)
	$(CC) $(CFLAGS) -o $(DEST)$(TARGET) $(SRC)

clean:
	rm -f $(DEST)$(TARGET)
