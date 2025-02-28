# filepath: /home/jacobj/projects/PiperC/makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = FractionTester
SRCS = FractionTester.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean