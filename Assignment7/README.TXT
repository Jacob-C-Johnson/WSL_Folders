2D Heat Transfer Simulation Project
This project implements a heat transfer simulation using a 9-point stencil operation with various parallel programming models. The code simulates heat diffusion on a 2D grid, saving frames at each iteration for visualization.

Project Structure
Assignment7/
├── code/
│   ├── stencil-2d.c          # Serial implementation
│   ├── stencil-2d-omp.c      # OpenMP implementation
│   ├── stencil-2d-pth.c      # Pthreads implementation
│   ├── stencil-2d-mpi.c      # MPI implementation
│   ├── stencil-2d-hybrid.c   # Hybrid MPI+OpenMP implementation
│   ├── make-2d.c             # Utility to create input matrices
│   ├── print-2d.c            # Utility to print matrices
│   ├── utilities.c/h         # Common utility functions
│   ├── timer.h               # Timing macros
│   ├── heatmap.py            # Python script for visualization
│   ├── makefile              # Compilation rules
│   ├── sbatch.sh             # SLURM batch script for experiments
│   ├── worker.sh             # Worker script for individual runs
│   └── driver.sh             # Driver script for submitting jobs
└── README.TXT                # This file

----------------------------------------------------------------------------------------------
Program Implementations:
Serial Version (stencil-2d.c)
    -Baseline implementation with no parallelization

OpenMP Version (stencil-2d-omp.c)
    -Parallelized using OpenMP directives with shared-memory model

Pthreads Version (stencil-2d-pth.c)
    -Parallelized using POSIX threads with manual work distribution

MPI Version (stencil-2d-mpi.c)
    -Parallelized using the Message Passing Interface for distributed computing
    -Distributes matrix rows across processes with ghost rows for communication

Hybrid Version (stencil-2d-hybrid.c)
    -Combines MPI and OpenMP for both distributed and shared-memory parallelism

----------------------------------------------------------------------------------------------
Algorithm:
The program implements a heat transfer simulation using a 9-point stencil:
    -Each inner cell's new value is calculated as the average of itself and its 8 neighboring cells
    -Boundary cells remain unchanged (fixed boundary conditions)
    -The stencil is applied in the order: NW+N+NE+E+SE+S+SW+W+C (Northwest, North, etc.)

----------------------------------------------------------------------------------------------
Complilation:
    cd code
    make

----------------------------------------------------------------------------------------------
Usages:
./stencil-2d <iterations> <input_file> <output_file> [verbosity]
./stencil-2d-omp <iterations> <input_file> <output_file> <verbosity> <num_threads>
./stencil-2d-pth <iterations> <input_file> <output_file> <verbosity> <num_threads>
./stencil-2d-mpi <iterations> <input_file> <output_file> <verbosity> <num_processes>
./stencil-2d-hybrid <iterations> <input_file> <output_file> <verbosity> <threads_per_process>

Examples:
./stencil-2d 10 input_matrix.bin output_matrix.bin 1
./stencil-2d-omp 10 input_matrix.bin output_matrix.bin 1 4

----------------------------------------------------------------------------------------------
visualization
python heatmap.py

----------------------------------------------------------------------------------------------
Running Experiments:
The project includes scripts for running performance experiments on a SLURM cluster

sbatch sbatch.sh    # Run full experiment suite
sbatch --export=IMPL="stencil-2d-omp",N=1000,P=4,T=10 worker.sh #Or for individual experiments