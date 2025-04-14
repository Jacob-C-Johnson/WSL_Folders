#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // Add this for mkdir
#include "utilities.h"

int main(int argc, char *argv[]) {
    // Parse command line arguments
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> [verbosity]\n", argv[0]);
        return 1;
    }

    int num_iterations = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    int verbosity = (argc > 4) ? atoi(argv[4]) : 0;  // Default verbosity 0 if not specified

    // Check if required arguments are provided
    if (num_iterations <= 0 || input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Error: Required arguments missing or invalid.\n");
        fprintf(stderr, "Usage: %s <num iters> <in> <out> <debug>\n", argv[0]);
        return 1;
    }
    
    // Read input matrix
    FILE *file = fopen(input_file, "rb");
    if (file == NULL) {
        perror("Error opening input file");
        return 1;
    }
    
    // Read dimensions
    int rows, cols;
    fread(&rows, sizeof(int), 1, file);
    fread(&cols, sizeof(int), 1, file);
    
    if (verbosity >= 1) {
        printf("Input matrix dimensions: %d x %d\n", rows, cols);
        printf("Number of iterations: %d\n", num_iterations);
    }
    
    // Allocate memory for matrices
    double *matrix_a = (double *)malloc(rows * cols * sizeof(double));
    double *matrix_b = (double *)malloc(rows * cols * sizeof(double));
    if (matrix_a == NULL || matrix_b == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(file);
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
        
        // Apply stencil (heat transfer calculation)
        apply_stencil(input, output, rows, cols);
        
        // Save frame with improved error checking
        if (save_frame(output, rows, cols, iter, frames_dir) != 0 && verbosity >= 1) {
            printf("Warning: Could not save frame %d\n", iter);
        }
        
        // Print state after iteration if verbosity is high
        if (verbosity >= 2) {
            printf("State after iteration %d:\n", iter);
            print_matrix_from_mem(output, rows, cols, iter);
        }
    }
    
    // Final result is in the output of the last iteration
    double *result = (num_iterations % 2 == 1) ? matrix_b : matrix_a;
    
    // Write output matrix
    file = fopen(output_file, "wb");
    if (file == NULL) {
        perror("Error opening output file");
        free(matrix_a);
        free(matrix_b);
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
        printf("Animation frames saved in '%s' directory\n", frames_dir);
    }
    
    // Clean up
    free(matrix_a);
    free(matrix_b);
    
    return 0;
}