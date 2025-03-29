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

char *int_to_binary(int n, int size) {
    //calculate the number of bits needed for the string, based on the np passed to the program by the user
    int bits = (int)log2(size);
    //allocate the memory needed for the string based on the num of bits, the null terminating char and the size of a character
    char* binary_str = (char*)malloc((bits + 1)*sizeof(char)); 
    if (binary_str == NULL) {
        return NULL; 
    }

    //sets index for the character array
    int index = 0;

    //loops through the array and evaluates using a bitwise operation if there 1 oe 0 is needed and adds that to the string.
    for (int i = bits - 1; i >= 0; i--){
        if(n & (1 << i)){
            binary_str[index] = '1';
        } else {
            binary_str[index] = '0';
        }
        index++;
    }

    //sets the null terminating char and returns the string
    binary_str[index] = '\0'; 
    return binary_str;
}


// Global Sum Version A: SPMD Reduction
void global_sumA(double* result, int rank, int size, double my_value, double* time_taken) {
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
    *time_taken = end_time - start_time; // Store the time taken
}

// Global Sum Version B: Tree-based Reduction
void global_sumB(double* result, int rank, int size, double my_value, double* time_taken) {
    double local_value = my_value;
    double recv_value;

    // Start timing
    double start_time = MPI_Wtime();

    // Reduction Phase: Compute the global sum
    for (int mask = 1; mask < size; mask <<= 1) {
        int partner = rank ^ mask;

        if ((rank & mask) == 0) {
            // Processes with rank & mask == 0 receive and add values
            if (partner < size) {
                MPI_Recv(&recv_value, 1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_value += recv_value;
            }
        } else {
            // Processes with rank & mask != 0 send their values and exit the loop
            MPI_Ssend(&local_value, 1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
            break;
        }
    }

    // Propagation Phase: Broadcast the global sum back to all processes
    for (int mask = size >> 1; mask > 0; mask >>= 1) {
        int partner = rank ^ mask;

        if ((rank & mask) == 0) {
            // Processes with rank & mask == 0 send the global sum to their partners
            if (partner < size) {
                MPI_Ssend(&local_value, 1, MPI_DOUBLE, partner, 1, MPI_COMM_WORLD);
            }
        } else {
            // Processes with rank & mask != 0 receive the global sum
            MPI_Recv(&local_value, 1, MPI_DOUBLE, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Store the global sum in the result
    *result = local_value;

    // End timing
    double end_time = MPI_Wtime();
    *time_taken = end_time - start_time; // Store the time taken
}