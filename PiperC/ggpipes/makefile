# Makefile for gg_pipes.c

CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = gg_pipes
SRC = gg_pipes.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean