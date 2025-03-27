/*
 * functions.h - Header file for global sum functions
 *
 * Author: Jacob Johnson
 * Date: 03/27/2025
 *
 * Assignment: Assignment 6
 * Course: CSCI 473
 * Version 1.0
 */


 #ifndef FUNCTIONS_H
 #define FUNCTIONS_H
 
 #include <mpi.h>
 
 // Function prototypes for global sum implementations
 void global_sumA(double* result, int rank, int size, double my_value);
 void global_sumB(double* result, int rank, int size, double my_value);

 
 // Helper function to check if number of processes is a power of 2
 int is_power_of_two(int n);
 
 #endif // FUNCTIONS_H