CFLAGS = -g -Wall -Wstrict-prototypes
PROGS = gsum
OBJECTS = gsum.o
LDFLAGS = -lm
CC = gcc
MCC = mpicc

all: $(PROGS)

gsum.o: gsum.c
	$(MCC) $(CFLAGS) -c gsum.c

functions.o: functions.c functions.h
	$(MCC) $(CFLAGS) -c functions.c

gsum: gsum.o functions.o
	$(MCC) $(CFLAGS) -o gsum gsum.o functions.o $(LDFLAGS)

clean:
	rm -f $(PROGS) $(OBJECTS) *.o core*