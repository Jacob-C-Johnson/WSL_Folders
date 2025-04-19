#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-6

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    FILE *file1 = fopen(argv[1], "rb");
    FILE *file2 = fopen(argv[2], "rb");
    if (!file1 || !file2) {
        perror("Error opening files");
        return 1;
    }

    int rows1, cols1, rows2, cols2;
    fread(&rows1, sizeof(int), 1, file1);
    fread(&cols1, sizeof(int), 1, file1);
    fread(&rows2, sizeof(int), 1, file2);
    fread(&cols2, sizeof(int), 1, file2);

    if (rows1 != rows2 || cols1 != cols2) {
        fprintf(stderr, "Matrix dimensions do not match: (%d x %d) vs (%d x %d)\n", rows1, cols1, rows2, cols2);
        return 1;
    }

    double val1, val2;
    for (int i = 0; i < rows1 * cols1; i++) {
        fread(&val1, sizeof(double), 1, file1);
        fread(&val2, sizeof(double), 1, file2);
        if (fabs(val1 - val2) > EPSILON) {
            fprintf(stderr, "Mismatch at index %d: %f vs %f\n", i, val1, val2);
            return 1;
        }
    }

    printf("Outputs match!\n");
    fclose(file1);
    fclose(file2);
    return 0;
}