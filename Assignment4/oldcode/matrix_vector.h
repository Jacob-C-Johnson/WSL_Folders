/*
 * matrix_vector.h - Header file for matrix vector multiplication functions
 *
 * Author: Jacob Johnson
 * Date: 02/08/2025
 *
 * Assignment: Assignment 3
 * Course: CSCI 473
 * Version 1.0
 */

 void Read_matrix_dimensions(const char* file_name, int* rows, int* cols);
 void Read_matrix_data(const char* file_name, double* matrix, int rows, int cols);
 void Write_matrix_to_file(const char* file_name, double* matrix, int rows, int cols);
 void Mat_vect_mult(double* A, double* x, double* y, int m, int n);