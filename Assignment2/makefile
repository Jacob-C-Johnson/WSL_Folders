CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = ComputePi

all: $(TARGET)

$(TARGET): ComputePi.o utilities.o
	$(CC) $(CFLAGS) -o $(TARGET) ComputePi.o utilities.o

ComputePi.o: ComputePi.c utilities.h
	$(CC) $(CFLAGS) -c ComputePi.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -f *.o $(TARGET)