#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // Add this for mkdir
#include <mpi.h> // Include MPI header
#include "utilities.h"
#include "timer.h"

int main(int argc, char *argv[]) {
    // Initialize MPI first
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Parse command line arguments
    if (argc != 6) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> <verbosity> <num_processors>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int num_iterations = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    int verbosity = atoi(argv[4]);
    int num_processors = atoi(argv[5]);

    // Check if required arguments are provided
    if (num_iterations <= 0 || input_file == NULL || output_file == NULL) {
        if (rank == 0) {
            fprintf(stderr, "Error: Required arguments missing or invalid.\n");
            fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> <verbosity> <num_processors>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    // Initialize timing variables
    double overall_start, overall_end, work_start, work_end, runTime, workTime;

    // Start overall timer
    GET_TIME(overall_start);
    
    double *matrix_a = NULL;
    double *matrix_b = NULL;
    int rows = 0, cols = 0;
    
    // Only rank 0 reads the file
    if (rank == 0) {
        // Read input matrix
        FILE *file = fopen(input_file, "rb");
        if (file == NULL) {
            perror("Error opening input file");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        // Read dimensions
        fread(&rows, sizeof(int), 1, file);
        fread(&cols, sizeof(int), 1, file);
        
        if (verbosity >= 1) {
            printf("Input matrix dimensions: %d x %d\n", rows, cols);
            printf("Number of iterations: %d\n", num_iterations);
        }
        
        // Allocate memory for matrices
        matrix_a = (double *)malloc(rows * cols * sizeof(double));
        matrix_b = (double *)malloc(rows * cols * sizeof(double));
        if (matrix_a == NULL || matrix_b == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        // Read the matrix data
        fread(matrix_a, sizeof(double), rows * cols, file);
        fclose(file);
        
        // Copy initial state to matrix_b
        memcpy(matrix_b, matrix_a, rows * cols * sizeof(double));
        
        // Create frames directory if it doesn't exist
        char frames_dir[256] = "frames";
        mkdir(frames_dir, 0777);  // Create directory for animation frames
        
        // Save initial state
        save_frame(matrix_a, rows, cols, 0, frames_dir);
        
        // Print initial state if verbosity is high
        if (verbosity >= 2) {
            printf("Initial state:\n");
            print_matrix_from_mem(matrix_a, rows, cols, 0);
        }
    }
    
    // Broadcast dimensions to all processes
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Other processes allocate memory now that they know dimensions
    if (rank != 0) {
        matrix_a = (double *)malloc(rows * cols * sizeof(double));
        matrix_b = (double *)malloc(rows * cols * sizeof(double));
        if (matrix_a == NULL || matrix_b == NULL) {
            fprintf(stderr, "Error: Memory allocation failed on process %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
    }
    
    // Broadcast initial matrix data to all processes
    MPI_Bcast(matrix_a, rows * cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrix_b, rows * cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Start work timer
    MPI_Barrier(MPI_COMM_WORLD);
    GET_TIME(work_start);

    // Perform iterations
    double *input, *output;
    for (int iter = 1; iter <= num_iterations; iter++) {
        // Swap input and output matrices in each iteration
        if (iter % 2 == 1) {
            input = matrix_a;
            output = matrix_b;
        } else {
            input = matrix_b;
            output = matrix_a;
        }
        
        // Apply stencil (heat transfer calculation) using MPI
        mpi_apply_stencil(input, output, rows, cols, rank, size);
        
        // Only rank 0 saves frames and prints
        if (rank == 0) {
            // Save frame with improved error checking
            if (save_frame(output, rows, cols, iter, "frames") != 0 && verbosity >= 1) {
                printf("Warning: Could not save frame %d\n", iter);
            }
            
            // Print state after iteration if verbosity is high
            if (verbosity >= 2) {
                printf("State after iteration %d:\n", iter);
                print_matrix_from_mem(output, rows, cols, iter);
            }
        }
    }
    
    // End work timer
    MPI_Barrier(MPI_COMM_WORLD);
    GET_TIME(work_end);
    
    // Only rank 0 saves final result
    if (rank == 0) {
        // Final result is in the output of the last iteration
        double *result = (num_iterations % 2 == 1) ? matrix_b : matrix_a;
        
        // Write output matrix
        FILE *file = fopen(output_file, "wb");
        if (file == NULL) {
            perror("Error opening output file");
            free(matrix_a);
            free(matrix_b);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        // Write dimensions and data
        fwrite(&rows, sizeof(int), 1, file);
        fwrite(&cols, sizeof(int), 1, file);
        fwrite(result, sizeof(double), rows * cols, file);
        
        fclose(file);
        
        if (verbosity >= 1) {
            printf("Heat transfer simulation completed.\n");
            printf("Output written to: %s\n", output_file);
            printf("Animation frames saved in 'frames' directory\n");
        }
    }
    
    // Clean up
    free(matrix_a);
    free(matrix_b);

    // End overall timer
    GET_TIME(overall_end);

    // Only rank 0 prints timing info
    if (rank == 0) {
        runTime = work_end - work_start;
        printf("Total time: %lf seconds\n", runTime);

        workTime = overall_end - overall_start;
        printf("Work time: %lf seconds\n", workTime);
    }
    
    // Finalize MPI
    MPI_Finalize();
    return 0;
}

// MPI version of apply_stencil
void mpi_apply_stencil(double *input, double *output, int rows, int cols, int rank, int size) {
    // Calculate work division for each process
    int elements_per_proc = (rows * cols) / size;
    int start_idx = rank * elements_per_proc;
    int end_idx = (rank == size - 1) ? (rows * cols) : (start_idx + elements_per_proc);

    // Process assigned elements
    for (int idx = start_idx; idx < end_idx; idx++) {
        int i = idx / cols;
        int j = idx % cols;
        
        // Don't update boundary cells
        if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
            output[idx] = input[idx];
            continue;
        }
        
        // 9-point stencil in the required order: NW+N+NE+E+SE+S+SW+W+C
        double NW = input[(i-1) * cols + (j-1)];  // Northwest
        double N  = input[(i-1) * cols + j];      // North
        double NE = input[(i-1) * cols + (j+1)];  // Northeast
        double E  = input[i * cols + (j+1)];      // East
        double SE = input[(i+1) * cols + (j+1)];  // Southeast
        double S  = input[(i+1) * cols + j];      // South
        double SW = input[(i+1) * cols + (j-1)];  // Southwest
        double W  = input[i * cols + (j-1)];      // West
        double C  = input[i * cols + j];          // Center
        
        // Calculate the average using the specified order
        output[idx] = (NW + N + NE + E + SE + S + SW + W + C) / 9.0;
    }
    
    // Create a temporary buffer for gathering results
    double *temp_output = NULL;
    if (rank == 0) {
        temp_output = (double *)malloc(rows * cols * sizeof(double));
        if (temp_output == NULL) {
            fprintf(stderr, "Error: Memory allocation failed in mpi_apply_stencil.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return;
        }
    }
    
    // Gather all results to rank 0
    MPI_Gather(output + start_idx, elements_per_proc, MPI_DOUBLE,
               temp_output, elements_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Rank 0 distributes the complete solution to all processes
    if (rank == 0) {
        // Copy the gathered data to the output array
        memcpy(output, temp_output, rows * cols * sizeof(double));
        free(temp_output);
    }
    
    // Broadcast the complete output to all processes
    MPI_Bcast(output, rows * cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}