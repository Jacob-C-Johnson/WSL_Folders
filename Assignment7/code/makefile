# Makefile for Assignment7
CC = gcc
CFLAGS = -Wall -g -pthread -fopenmp
LDFLAGS = -lm
TARGETS = make-2d print-2d stencil-2d stencil-2d-pth stencil-2d-omp
OBJS = make-2d.o print-2d.o stencil-2d.o stencil-2d-pth.o stencil-2d-omp.o utilities.o

all: $(TARGETS)

make-2d: make-2d.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

print-2d: print-2d.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d: stencil-2d.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d-pth: stencil-2d-pth.o utilities.o
	$(CC) $(CFLAGS) -pthread -o $@ $^ $(LDFLAGS)

stencil-2d-omp: stencil-2d-omp.o utilities.o
	$(CC) $(CFLAGS) -fopenmp -o $@ $^ $(LDFLAGS)


%.o: %.c utilities.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g
debug: clean all

clean:
	rm -f $(TARGETS) $(OBJS)

help:
	@echo "Usage:"
	@echo "  make          - Build all targets"
	@echo "  make debug    - Build all targets with debugging symbols"
	@echo "  make clean    - Remove all generated files"