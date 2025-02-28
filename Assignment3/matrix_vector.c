/*
 * matrix_vector.c - Program to perform matrix-vector multiplication
 *
 * Author: Jacob Johnson
 * Date: 02/08/2025
 *
 * Assignment: Assignment 3
 * Course: CSCI 473
 * Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "matrix_vector.h"
#include "timer.h"

// Timing variables
double start, finish;
double workStart, workFinish;
double runTime, workTime;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./matrix_vector <file A> <file x> <file y>\n");
        exit(EXIT_FAILURE);
    }
    GET_TIME(start); // start timer

    const char* file_A = argv[1];
    const char* file_x = argv[2];
    const char* file_y = argv[3];

    int rows_A, cols_A, rows_x, cols_x;

    // Read dimensions from binary files
    Read_matrix_dimensions(file_A, &rows_A, &cols_A);
    Read_matrix_dimensions(file_x, &rows_x, &cols_x);

    // Error check: Ensure matrix-vector multiplication compatibility
    if (cols_A != rows_x || cols_x != 1) {
        fprintf(stderr, "Matrix A (%d x %d) and vector x (%d x %d) are incompatible for multiplication\n", 
                rows_A, cols_A, rows_x, cols_x);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for matrices and vectors
    double* A = malloc(rows_A * cols_A * sizeof(double));
    double* x = malloc(rows_x * sizeof(double));
    double* y = malloc(rows_A * sizeof(double));

    if (A == NULL || x == NULL || y == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Read the matrix and vector data from files
    Read_matrix_data(file_A, A, rows_A, cols_A);
    Read_matrix_data(file_x, x, rows_x, cols_x);

    // Perform matrix-vector multiplication
    GET_TIME(workStart); // start work timer
    Mat_vect_mult(A, x, y, rows_A, cols_A);
    GET_TIME(workFinish); // end work timer

    // Write result to output file
    Write_matrix_to_file(file_y, y, rows_A, 1);

    GET_TIME(finish); // end timer

    runTime = finish - start;
    printf("Total time: %lf seconds\n", runTime);

    workTime = workFinish - workStart;
    printf("Work time: %lf seconds\n", workTime);



    // Clean up memory
    free(A);
    free(x);
    free(y);

    return 0;
}

void Read_matrix_dimensions(const char* file_name, int* rows, int* cols) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (fread(rows, sizeof(int), 1, file) != 1 || fread(cols, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Error reading matrix dimensions\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void Read_matrix_data(const char* file_name, double* matrix, int rows, int cols) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Skip the first 8 bytes (rows and cols)
    fseek(file, 2 * sizeof(int), SEEK_SET);

    if (fread(matrix, sizeof(double), rows * cols, file) != (size_t)(rows * cols)) {
        fprintf(stderr, "Error reading matrix data\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void Write_matrix_to_file(const char* file_name, double* matrix, int rows, int cols) {
    FILE* file = fopen(file_name, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Write dimensions
    fwrite(&rows, sizeof(int), 1, file);
    fwrite(&cols, sizeof(int), 1, file);

    // Write matrix data
    fwrite(matrix, sizeof(double), rows * cols, file);

    fclose(file);
}

void Mat_vect_mult(double* A, double* x, double* y, int m, int n) {
    for (int i = 0; i < m; i++) {
        y[i] = 0.0;
        for (int j = 0; j < n; j++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}
