#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // Add this for mkdir
#include <pthread.h> // Pthreads header
#include "utilities.h"
#include "timer.h"

pthread_t *thread_handles;
int num_processors;

int main(int argc, char *argv[]) {
    // Parse command line arguments
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> <verbosity> <num_processors>\n", argv[0]);
        return 1;
    }

    int num_iterations = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    int verbosity = atoi(argv[4]);
    num_processors = atoi(argv[5]);

    // Check if required arguments are provided
    if (num_iterations <= 0 || input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Error: Required arguments missing or invalid.\n");
        fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> <verbosity> <num_processors>\n", argv[0]);
        return 1;
    }
    
    // Set the number of threads for Pthreads
    thread_handles = malloc(num_processors * sizeof(pthread_t));

    // Initialize timing variables
    double overall_start, overall_end, work_start, work_end, runTime, workTime;

    // Start overall timer
    GET_TIME(overall_start);

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
    
    // Start work timer
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
        
        // Apply stencil (heat transfer calculation)
        pth_apply_stencil(input, output, rows, cols);
        
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
    
    // End work timer
    GET_TIME(work_end);
    

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

    // End overall timer
    GET_TIME(overall_end);

    runTime = work_end - work_start;
    printf("Total time: %lf seconds\n", runTime);

    workTime = overall_end - overall_start;
    printf("Work time: %lf seconds\n", workTime);
    
    return 0;
}

// Thread data structure
typedef struct {
    double* input;
    double* output;
    int rows;
    int cols;
    int thread_id;
    int num_threads;
} ThreadData;

// Thread function
void* thread_stencil_work(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int rows = data->rows;
    int cols = data->cols;
    double* input = data->input;
    double* output = data->output;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;
    
    // Calculate work division - use row-based division
    int total_rows = rows - 2; // Exclude the boundary rows
    int start_row = BLOCK_LOW(thread_id, num_threads, total_rows) + 1; // +1 to skip first boundary row
    int end_row = BLOCK_HIGH(thread_id, num_threads, total_rows) + 1;  // +1 to skip first boundary row
    
    // Process assigned rows
    for (int i = start_row; i <= end_row; i++) {
        for (int j = 1; j < cols-1; j++) { // Skip boundary columns
            int index = i * cols + j;
            
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
            output[index] = (NW + N + NE + E + SE + S + SW + W + C) / 9.0;
        }
    }
    
    // Process boundary cells (just copy) - only if this thread handles the boundary
    if (thread_id == 0) {
        // Top row
        for (int j = 0; j < cols; j++) {
            output[j] = input[j];
        }
    }
    
    if (thread_id == num_threads - 1) {
        // Bottom row
        for (int j = 0; j < cols; j++) {
            output[(rows-1) * cols + j] = input[(rows-1) * cols + j];
        }
    }
    
    // Left and right columns (for the rows this thread handles)
    for (int i = start_row; i <= end_row; i++) {
        output[i * cols] = input[i * cols];               // Left column
        output[i * cols + (cols-1)] = input[i * cols + (cols-1)];  // Right column
    }
    
    return NULL;
}

// Modified pth_apply_stencil function
void pth_apply_stencil(double *input, double *output, int rows, int cols) {
    // Create an array for thread data
    ThreadData* thread_data = malloc(num_processors * sizeof(ThreadData));
    
    // Initialize thread data and create threads
    for (int i = 0; i < num_processors; i++) {
        thread_data[i].input = input;
        thread_data[i].output = output;
        thread_data[i].rows = rows;
        thread_data[i].cols = cols;
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_processors;
        
        pthread_create(&thread_handles[i], NULL, thread_stencil_work, &thread_data[i]);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < num_processors; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    
    // Free the thread data
    free(thread_data);
}