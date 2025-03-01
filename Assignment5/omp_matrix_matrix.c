/*
 * omp_matrix_matrix.c - Parallel matrix-matrix multiplication using OpenMP
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
#include <omp.h>
#include "timer.h"
#include "omp_matrix_matrix.h"

// Global Variables
double *A, *x, *y;
int m, n, p;
int thread_count;

// Timing variables
double start, finish;
double workStart, workFinish;
double runTime, workTime;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: ./omp_matrix_matrix <file A> <file X> <file Y> <threads>\n");
        exit(1);
    }

    GET_TIME(start);

    const char* fileA = argv[1];
    const char* fileX = argv[2];
    const char* fileY = argv[3];
    thread_count = atoi(argv[4]);

    int rows_A, cols_A, rows_x, cols_x;

    // Read dimensions from binary files
    Read_matrix_dimensions(fileA, &rows_A, &cols_A);
    Read_matrix_dimensions(fileX, &rows_x, &cols_x);

    // Error check: Ensure matrix-matrix multiplication compatibility
    if (cols_A != rows_x) {
        fprintf(stderr, "Matrix A (%d x %d) and Matrix X (%d x %d) are incompatible for multiplication\n", 
                rows_A, cols_A, rows_x, cols_x);
        exit(EXIT_FAILURE);
    }

    m = rows_A;
    n = cols_A;
    p = cols_x;

    // Allocate memory for matrices and vectors
    A = malloc(m * n * sizeof(double));
    x = malloc(rows_x * cols_x * sizeof(double));
    y = malloc(m * p * sizeof(double));  // Instead of m * sizeof(double)

    if (A == NULL || x == NULL || y == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Read the matrix and vector data from files
    Read_matrix_data(fileA, A, m, n);
    Read_matrix_data(fileX, x, rows_x, cols_x);

    // Set the number of threads
    omp_set_num_threads(thread_count);

    // Parallel matrix-matrix multiplication
    GET_TIME(workStart); // Start work timer
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            y[i * p + j] = 0.0;  // Initialize result cell
            for (int k = 0; k < n; k++) {  // Compute dot product
                y[i * p + j] += A[i * n + k] * x[k * p + j];
            }
        }
    }
    GET_TIME(workFinish); // End work timer

    // Write the result vector to file Y
    Write_matrix_to_file(fileY, y, m, p);

    // Cleanup
    if (A != NULL) {
        free(A);
        A = NULL;
    }
    if (x != NULL) {
        free(x);
        x = NULL;
    }
    if (y != NULL) {
        free(y);
        y = NULL;
    }

    GET_TIME(finish);

    runTime = finish - start;
    printf("Total time: %lf seconds\n", runTime);

    workTime = workFinish - workStart;
    printf("Work time: %lf seconds\n", workTime);

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