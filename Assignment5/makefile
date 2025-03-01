CC = gcc
CFLAGS = -Wall -g -pthread -fopenmp
TARGETS = make_matrix print_matrix matrix_matrix omp_matrix_matrix

# Object files for each target
MAKE_MATRIX_OBJS = make_matrix.o
PRINT_MATRIX_OBJS = print_matrix.o
MATRIX_MATRIX_OBJS = matrix_matrix.o
OMP_MATRIX_MATRIX_OBJS = omp_matrix_matrix.o

all: $(TARGETS)

# Rules to build the make_matrix executable
make_matrix: $(MAKE_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the print_matrix executable
print_matrix: $(PRINT_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the matrix_matrix executable
matrix_matrix: $(MATRIX_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the omp_matrix_matrix executable
omp_matrix_matrix: $(OMP_MATRIX_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile rules for object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGETS) *.o
