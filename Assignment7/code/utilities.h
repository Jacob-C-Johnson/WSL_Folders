// make matrix utilities
void make_matrix(const char *file_name, int rows, int cols);

// print matrix utilities
void Print_matrix(const char *file_name);
void print_matrix_from_mem(double *matrix, int rows, int cols, int iteration);

// stencil-2d utilities
double stencil(int r, int cols, int c, double *matrix);
void apply_stencil(double *input, double *output, int rows, int cols);
int save_frame(double *matrix, int rows, int cols, int iteration, const char *output_dir);

// Function prototype for omp_apply_stencil
void omp_apply_stencil(double *input, double *output, int rows, int cols);

// pthreads utilities
void pth_apply_stencil(double *input, double *output, int rows, int cols); // Function prototype for pth_apply_stencil
#define BLOCK_LOW(id,p,n) ((id)*(n)/(p)) // given rank = id, give p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you the starting index
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1) // given rank = id, give p = # processes (or threads), and given n, number of elements in 1 dimension, it will tell you the ending index

// MPI utilities
void hybrid_apply_stencil(double *input, double *output, int local_rows, int cols, int rank, int size);
void mpi_apply_stencil_local(double *input, double *output, int local_rows, int cols, int rank, int size);