Assignment6/
├── MPI Global Summations.pptx   # Presentation explaining the algorithms
├── README.TXT                   # This README file
├── code/                        # Source code and scripts
│   ├── functions.c              # Implementation of global_sumA and global_sumB
│   ├── functions.h              # Header file for function prototypes
│   ├── gsum.c                   # Main program to test the global summation functions
│   ├── makefile                 # Makefile to compile the program
│   ├── run_tests.sh             # Script to automate testing with different process counts
│   ├── sbatch.bash              # Slurm batch script to run the program on a cluster
├── data/                        # Output files from test runs
│   ├── ComputeTest.37833608.exp-3-41.out
│   ├── ComputeTest.37833609.exp-4-25.out
│   ├── ComputeTest.37833610.exp-4-31.out
│   ├── ComputeTest.37833611.exp-4-10.out


How to Compile:
    
    Load Required Modules: Ensure the necessary modules are loaded (e.g., gcc, openmpi):
    - module load gcc/10.2.0
    - module load openmpi/4.1.3

    Compile the Program: Use the provided makefile to compile the program:
        cd code/
        make

How to Run:

    You can run the program locally using mpirun: 
        mpirun -np <num_processes> ./gsum

    Run on a cluster (slurm)
        sbatch sbatch.bash

Testing:

    Automated Testing:
        Use the run_tests.sh script to test the program with different numbers of processes (e.g., 2, 4, 8, 16):
            bash run_tests.sh


Output Format:

    Each process will print its computed global sum and the time taken for both implementations:
        FINAL IN MAIN: Process: A <rank> has Sum = <sum_a> with time = <time_a>
        FINAL IN MAIN: Process: B <rank> has Sum = <sum_b> with time = <time_b>