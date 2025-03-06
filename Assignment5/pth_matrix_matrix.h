/*
 * pth_matrix_vector.h - Header file for parallel matrix vector multiplication functions
 *
 * Author: Jacob Johnson
 * Date: 02/08/2025
 *
 * Assignment: Assignment 3
 * Course: CSCI 473
 * Version 1.0
 */
#define MIN(a,b) ((a)<(b)?(a):(b))

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p)) // given rank = id, give p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you the starting index

#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1) // given rank = id, give p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you the ending index

#define BLOCK_SIZE(id,p,n) \
(BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1) // given rank = id, give p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you number of elements that the given thread or process is responsible for

#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n)) // given data index j (from [0, n-1], and given p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you which RANK is responsible for that particular element, j.
#define PTR_SIZE (sizeof(void*))
#define CEILING(i,j) (((i)+(j)-1)/(j))

void* pth_mat_vect(void* rank);

void Read_matrix_dimensions(const char* file_name, int* rows, int* cols);
void Read_matrix_data(const char* file_name, double* matrix, int rows, int cols);
void Write_matrix_to_file(const char* file_name, double* matrix, int rows, int cols);
void Mat_vect_mult(double* A, double* x, double* y, int m, int n);