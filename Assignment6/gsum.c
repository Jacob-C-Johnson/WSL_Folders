#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "functions.h"

int main(int argc, char** argv) {
    int rank, size;
    double my_value, result_a, result_b;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if number of processes is a power of 2
    if ((size & (size - 1)) != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: Number of processes must be a power of 2\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Each process initializes its my_value to its rank
    my_value = (double)rank;

    // Compute global sum using Version A
    global_sumA(&result_a, rank, size, my_value);

    // Compute global sum using Version B
    global_sumB(&result_b, rank, size, my_value);

    // Print results
    printf("FINAL IN MAIN: Process: %d has Sum = %f with time = %f \n", rank, result_a, MPI_Wtime());
    printf("FINAL IN MAIN: Process: %d has Sum = %f with time = %f \n", rank, result_b, MPI_Wtime());


    // Finalize MPI
    MPI_Finalize();
    return 0;
}