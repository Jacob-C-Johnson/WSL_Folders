#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include "utilities.h"

// print-2d.c utilities
void Print_matrix(const char *file_name){
    // Open the file for reading in binary mode
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Read the matrix dimensions (first 8 bytes)
    int rows, cols;
    if (fread(&rows, sizeof(int), 1, file) != 1 || fread(&cols, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error reading matrix dimensions\n");
        fclose(file);
        return;
    }

    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Invalid matrix dimensions\n");
        fclose(file);
        return;
    }

    // Allocate memory for the matrix
    double *matrix = (double *)malloc(rows * cols * sizeof(double));
    if (matrix == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(file);
        return;
    }

    // Read the matrix from the file
    if (fread(matrix, sizeof(double), rows * cols, file) != (size_t)(rows * cols)) {
        fprintf(stderr, "Error reading matrix data\n");
        free(matrix);
        fclose(file);
        return;
    }

    // Print the matrix in a nicely formatted way
    printf("Matrix (%d x %d):\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%05.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }

    // Clean up
    free(matrix);
    fclose(file);
}

void print_matrix_from_mem(double *matrix, int rows, int cols, int iteration) {
    printf("Matrix at iteration %d:\n", iteration);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%05.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

//stensil-2d.c utilities
void apply_stencil(double *input, double *output, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            
            // Don't update boundary cells
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
                output[index] = input[index];
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
            output[index] = (NW + N + NE + E + SE + S + SW + W + C) / 9.0;
        }
    }
}

// heatmap functions
int save_frame(double *matrix, int rows, int cols, int frame, const char *output_dir) {
    char filename[256];
    sprintf(filename, "%s/frame%d", output_dir, frame);
    
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return 1; // Return error code
    }
    
    // Write dimensions and matrix data
    if (fwrite(&rows, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error writing rows to file %s\n", filename);
        fclose(file);
        return 2;
    }
    
    if (fwrite(&cols, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error writing cols to file %s\n", filename);
        fclose(file);
        return 3;
    }
    
    if (fwrite(matrix, sizeof(double), rows * cols, file) != rows * cols) {
        fprintf(stderr, "Error writing matrix data to file %s\n", filename);
        fclose(file);
        return 4;
    }
    
    // Ensure data is written to disk
    fflush(file);
    fclose(file);
    
    return 0; // Success
}