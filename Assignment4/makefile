CC = gcc
CFLAGS = -Wall -g -pthread
TARGETS = make_matrix print_matrix matrix_vector pth_matrix_vector

# Object files for each target
MAKE_MATRIX_OBJS = make_matrix.o
PRINT_MATRIX_OBJS = print_matrix.o
MATRIX_VECTOR_OBJS = matrix_vector.o
PTH_MATRIX_VECTOR_OBJS = pth_matrix_vector.o

all: $(TARGETS)

# Rules to build the make_matrix executable
make_matrix: $(MAKE_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the print_matrix executable
print_matrix: $(PRINT_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the matrix_vector executable
matrix_vector: $(MATRIX_VECTOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rules to build the pth_matrix_vector executable
pth_matrix_vector: $(PTH_MATRIX_VECTOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile rules for object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGETS) *.o
