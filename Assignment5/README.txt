Matrix Multiplication Performance Analysis

This project implements and compares serial, OpenMP-parallel, and Pthreads-parallel versions of matrix-matrix multiplication algorithms.

Files:
    - make_matrix: Creates a binary file that contains a matrix of specified dimensions
    - print_matrix: Displays the contents of a binary file that contains a matrix
    - matrix_matrix: Serial implemntation of matrix matrix multiplication
    - omp_matrix_matrix: OpenMP parallel implementation of matrix-matrix multiplication
    - pth_matrix_matrix.c: Pthreads parallel implementation of matrix-matrix multiplication
    - sbatch.sh: SLURM batch script for running performance experiments on an HPC cluster
    - Header files (.h): Contain function declarations and macros

Easy Compliation with a makefile:
    This project includes a makefile that compiles every file for the user

    Example run in the terminal
        make all       # Compile all programs
        make clean     # Remove all executables and object files
        make test      # Will run the test compilation

Usages:
    ./make_matrix <filename> <rows> <cols> 
    ./print_matrix <filename>
    ./matrix_matrix <matrix_A> <matrix_X> <output_Y>
    ./omp_matrix_matrix <matrix_A> <matrix_X> <output_Y> <threads>
    ./pth_matrix_matrix <matrix_A> <matrix_X> <output_Y> <threads>

Running Preformance Tests:
    The sbatch.sh script runs a series of tests on the HPC cluster:
    
    Steps:
        1. Tests serial implementation with matrix sizes: 256, 512, 1024, 2048, 4096, 8192
        2. Tests parallel implementations with the same matrix sizes and thread counts: 1, 2, 4, 8, 16, 32, 64, 128
        3. Records results in a CSV file for analysis

    Run the Tests:
        To submit the job to SLURM:
            1. sbatch sbatch.sh
        Monitor job status with:
            squeue -u <username>
            watch squeue -u <username>
    
    Test Results:
        The script generates a results.csv file containing:
        - Experiment number
        - Implementation type (Serial/Parallel)
        - Matrix size (N)
        - Thread count (P)
        - Total execution time
        - Work time (computation only)

Terminal Testing:
    To validate that all implementations produce the same result:
        make test # This creates test matrices, multiplies them using all three implementations, and prints the results for comparison.
