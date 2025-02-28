/*
 * print_matrix.c - Implementation of print matrix functions
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
#include "print_matrix.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    const char *file_name = argv[1];
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
     
    Print_matrix(file_name);
    fclose(file);
    

    return 0;
}

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
