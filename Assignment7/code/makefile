# Makefile for Assignment7
CC = gcc
MCC = mpicc # for MPI files
CFLAGS = -Wall -g -pthread -fopenmp
LDFLAGS = -lm
TARGETS = make-2d print-2d stencil-2d stencil-2d-pth stencil-2d-omp stencil-2d-mpi stencil-2d-hybrid compare-outs
OBJS = make-2d.o print-2d.o stencil-2d.o stencil-2d-pth.o stencil-2d-omp.o stencil-2d-mpi.o stencil-2d-hybrid.o compare-outs.o utilities.o 

all: $(TARGETS)

make-2d: make-2d.o utilities.o
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

print-2d: print-2d.o utilities.o
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d: stencil-2d.o utilities.o
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d-pth: stencil-2d-pth.o utilities.o
	$(MCC) $(CFLAGS) -pthread -o $@ $^ $(LDFLAGS)

stencil-2d-omp: stencil-2d-omp.o utilities.o
	$(MCC) $(CFLAGS) -fopenmp -o $@ $^ $(LDFLAGS)

# specific compilation rules for mpi
stencil-2d-mpi: stencil-2d-mpi.o utilities.c
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d-mpi.o: stencil-2d-mpi.c utilities.h
	$(MCC) $(CFLAGS) -c $< -o $@

stencil-2d-hybrid: stencil-2d-hybrid.o utilities.o
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d-hybrid.o: stencil-2d-hybrid.c utilities.h
	$(MCC) $(CFLAGS) -c $< -o $@

# testing scripts

compare-outs: compare-outs.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


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