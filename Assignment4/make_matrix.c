/*
 * make_matrix.c - Program to create a binary file with a matrix of random doubles
 *
 * Author: Jacob Johnson
 * Date: 02/08/2025
 *
 * Assignment: Assignment 3
 * Course: CSCI 473
 * Version 1.0
 */
#include "make_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    // Fill the matrix with random double values with two places before and after the decimal
    for (int i = 0; i < rows * cols; i++) {
        int integer_part = rand() % 100; // Random integer part between 00 and 99
        int decimal_part = rand() % 100; // Random decimal part between 00 and 99
        matrix[i] = integer_part + decimal_part / 100.0;
    }

    // Write the matrix to the file
    fwrite(matrix, sizeof(double), rows * cols, file);

    // Clean up
    free(matrix);
    fclose(file);
}
