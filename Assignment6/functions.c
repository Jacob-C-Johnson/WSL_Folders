/*
* functions.c - implementation file for global sum functions
*
* Author: Jacob Johnson
* Date: 03/27/2025
*
* Assignment: Assignment 6
* Course: CSCI 473
* Version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "functions.h"

// Helper function to check if number of processes is a power of 2
int is_power_of_two(int n) {
    // A power of 2 has only one bit set in its binary representation
    return n > 0 && (n & (n - 1)) == 0;
}

// Global Sum Version A: SPMD Reduction
void global_sumA(double* result, int rank, int size, double my_value) {
    double local_value = my_value;
    double recv_value;
    int i;

    // Start timing
    double start_time = MPI_Wtime();

    if (rank == 0) {
        // Process 0 collects values and computes the sum
        *result = local_value;
        for (i = 1; i < size; i++) {
            MPI_Recv(&recv_value, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            *result += recv_value;
        }
        // Send the result to all other processes
        for (i = 1; i < size; i++) {
            MPI_Ssend(result, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
    } else {
        // Other processes send their value to process 0
        MPI_Ssend(&local_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        // Receive the global sum from process 0
        MPI_Recv(result, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // End timing
    double end_time = MPI_Wtime();

    // Print timing information
    printf("Process %d: Time taken for global_sumA = %f seconds\n", rank, end_time - start_time);
}

// Global Sum Version B: Tree-based Reduction
void global_sumB(double* result, int rank, int size, double my_value) {
    double local_value = my_value;
    double recv_value;

    // Start timing
    double start_time = MPI_Wtime();

    // Tree-based reduction to compute the global sum
    int step;
    for (step = 1; step < size; step *= 2) {
        if (rank % (2 * step) == 0) {
            // Receive value from a neighbor
            if (rank + step < size) {
                MPI_Recv(&recv_value, 1, MPI_DOUBLE, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_value += recv_value;
            }
        } else {
            // Send value to a neighbor
            MPI_Ssend(&local_value, 1, MPI_DOUBLE, rank - step, 0, MPI_COMM_WORLD);
            break;
        }
    }

    // Propagate the result back down the tree
    for (step /= 2; step > 0; step /= 2) {
        if (rank % (2 * step) == 0) {
            if (rank + step < size) {
                MPI_Ssend(&local_value, 1, MPI_DOUBLE, rank + step, 1, MPI_COMM_WORLD);
            }
        } else {
            MPI_Recv(&local_value, 1, MPI_DOUBLE, rank - step, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    *result = local_value; // Store final result
    

    *result = local_value;

    // End timing
    double end_time = MPI_Wtime();

    // Print timing information
    printf("Process %d: Time taken for global_sumB = %f seconds\n", rank, end_time - start_time);
}