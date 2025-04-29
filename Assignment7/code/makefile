CC = gcc
MCC = mpicc  # for MPI files
CFLAGS = -Wall -g -pthread -fopenmp
LDFLAGS = -lm

TARGETS = make-2d print-2d stencil-2d stencil-2d-pth stencil-2d-omp stencil-2d-mpi stencil-2d-hybrid compare-outs
OBJS = make-2d.o print-2d.o stencil-2d.o stencil-2d-pth.o stencil-2d-omp.o stencil-2d-mpi.o stencil-2d-hybrid.o compare-outs.o utilities.o

all: $(TARGETS)

# Non-MPI Targets (compiled with gcc)
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

compare-outs: compare-outs.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# MPI Targets (compiled with mpicc)
stencil-2d-mpi: stencil-2d-mpi.o utilities.o
	$(MCC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stencil-2d-mpi.o: stencil-2d-mpi.c utilities.h
	$(MCC) $(CFLAGS) -c $< -o $@

stencil-2d-hybrid: stencil-2d-hybrid.o utilities.o
	$(MCC) $(CFLAGS) -fopenmp -o $@ $^ $(LDFLAGS)

stencil-2d-hybrid.o: stencil-2d-hybrid.c utilities.h
	$(MCC) $(CFLAGS) -c $< -o $@

# Object file rule
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


# Parameters for testing
ITER = 100
INPUT = input.txt
VERB = 0
NPROCS = 4

test: stencil-2d stencil-2d-pth stencil-2d-omp stencil-2d-mpi stencil-2d-hybrid print-2d compare-outs
    @echo "Running sequential version..."
    ./stencil-2d $(ITER) $(INPUT) out-seq.txt $(VERB)
    @echo "Output (Sequential):"
    ./print-2d out-seq.txt

    @echo "Running pthreads version..."
    ./stencil-2d-pth $(ITER) $(INPUT) out-pth.txt $(VERB) $(NPROCS)
    @echo "Output (Pthreads):"
    ./print-2d out-pth.txt

    @echo "Running OpenMP version..."
    ./stencil-2d-omp $(ITER) $(INPUT) out-omp.txt $(VERB) $(NPROCS)
    @echo "Output (OpenMP):"
    ./print-2d out-omp.txt

    @echo "Running MPI version..."
    mpirun -np $(NPROCS) ./stencil-2d-mpi $(ITER) $(INPUT) out-mpi.txt $(VERB) $(NPROCS)
    @echo "Output (MPI):"
    ./print-2d out-mpi.txt

    @echo "Running Hybrid version..."
    mpirun -np $(NPROCS) ./stencil-2d-hybrid $(ITER) $(INPUT) out-hybrid.txt $(VERB) $(NPROCS)
    @echo "Output (Hybrid):"
    ./print-2d out-hybrid.txt

    @echo "Comparing outputs with sequential baseline..."
    @./compare-outs out-seq.txt out-pth.txt || echo "Mismatch: Pthreads"
    @./compare-outs out-seq.txt out-omp.txt || echo "Mismatch: OpenMP"
    @./compare-outs out-seq.txt out-mpi.txt || echo "Mismatch: MPI"
    @./compare-outs out-seq.txt out-hybrid.txt || echo "Mismatch: Hybrid"

    @echo "Test complete."