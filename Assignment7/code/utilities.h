
// make matrix utilities
void make_matrix(const char *file_name, int rows, int cols);

// print matrix utilities
void Print_matrix(const char *file_name);

// stencil-2d utilities
double stencil(int r, int cols, int c, double *matrix);
void apply_stencil(double *input, double *output, int rows, int cols);
void save_frame(double *matrix, int rows, int cols, int iteration, const char *output_dir);