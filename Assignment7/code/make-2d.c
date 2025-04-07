#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utilities.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <file_name> <rows> <cols>\n", argv[0]);
        return 1;
    }
    
    const char *file_name = argv[1];
    int rows = atoi(argv[2]);
    int cols = atoi(argv[3]);
    make_matrix(file_name, rows, cols);

    return 0;
}

void make_matrix(const char *file_name, int rows, int cols) {
    // Open the file for writing in binary mode
    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write the dimensions (4 bytes each for int rows and cols)
    fwrite(&rows, sizeof(int), 1, file);
    fwrite(&cols, sizeof(int), 1, file);

    // Allocate memory for the matrix
    double *matrix = (double *)malloc(rows * cols * sizeof(double));
    if (matrix == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(file);
        return;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Initialize the matrix with boundary conditions
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            
            if (j == 0 || j == cols - 1) {
                // Left and right walls are 1.0
                matrix[index] = 1.0;
            } else if (i == 0 || i == rows - 1) {
                // Top and bottom walls are 0.0
                matrix[index] = 0.0;
            } else {
                // Inner values start at 0.0
                matrix[index] = 0.0;
            }
        }
    }

    // Write the matrix to the file
    fwrite(matrix, sizeof(double), rows * cols, file);

    // Clean up
    free(matrix);
    fclose(file);
}
