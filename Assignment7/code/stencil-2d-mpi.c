#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <mpi.h>
#include "utilities.h"
#include "timer.h"

void mpi_apply_stencil_local(double *input, double *output, int local_rows, int cols, int rank, int size);

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 6) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <num_iterations> <input_file> <output_file> <verbosity> <num_processors>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int num_iterations = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    int verbosity = atoi(argv[4]);
    int num_processors = atoi(argv[5]);

    double overall_start, overall_end, work_start, work_end;

    GET_TIME(overall_start);

    int rows = 0, cols = 0;
    double *full_matrix = NULL;

    if (rank == 0) {
        FILE *file = fopen(input_file, "rb");
        if (!file) {
            perror("Error opening input file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fread(&rows, sizeof(int), 1, file);
        fread(&cols, sizeof(int), 1, file);

        if (verbosity >= 1) {
            printf("Input matrix dimensions: %d x %d\n", rows, cols);
            printf("Number of iterations: %d\n", num_iterations);
        }

        full_matrix = malloc(rows * cols * sizeof(double));
        if (!full_matrix) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fread(full_matrix, sizeof(double), rows * cols, file);
        fclose(file);

        mkdir("frames", 0777);
        save_frame(full_matrix, rows, cols, 0, "frames");
    }

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_rows = rows / size;
    if (rank == size - 1) {
        local_rows += rows % size; // Last rank takes the remainder
    }

    double *local_a = malloc((local_rows + 2) * cols * sizeof(double)); // +2 for ghost rows
    double *local_b = malloc((local_rows + 2) * cols * sizeof(double));
    if (!local_a || !local_b) {
        fprintf(stderr, "Rank %d: Error allocating local matrices\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double *sendbuf = NULL;
    int *sendcounts = NULL, *displs = NULL;

    if (rank == 0) {
        sendbuf = full_matrix;
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));

        int disp = 0;
        for (int i = 0; i < size; i++) {
            int rows_i = rows / size;
            if (i == size - 1) rows_i += rows % size;
            sendcounts[i] = rows_i * cols;
            displs[i] = disp;
            disp += rows_i * cols;
        }
    }

    MPI_Scatterv(sendbuf, sendcounts, displs, MPI_DOUBLE,
                 &local_a[cols], local_rows * cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    memcpy(&local_b[cols], &local_a[cols], local_rows * cols * sizeof(double));

    MPI_Barrier(MPI_COMM_WORLD);
    GET_TIME(work_start);

    for (int iter = 1; iter <= num_iterations; iter++) {
        double *input = (iter % 2 == 1) ? local_a : local_b;
        double *output = (iter % 2 == 1) ? local_b : local_a;

        mpi_apply_stencil_local(input, output, local_rows, cols, rank, size);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    GET_TIME(work_end);

    MPI_Gatherv(&local_a[cols], local_rows * cols, MPI_DOUBLE,
                full_matrix, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        FILE *outfile = fopen(output_file, "wb");
        if (!outfile) {
            perror("Error opening output file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fwrite(&rows, sizeof(int), 1, outfile);
        fwrite(&cols, sizeof(int), 1, outfile);
        fwrite(full_matrix, sizeof(double), rows * cols, outfile);
        fclose(outfile);

        printf("Heat transfer simulation completed.\n");
        printf("Output written to: %s\n", output_file);
    }

    free(local_a);
    free(local_b);
    if (rank == 0) free(full_matrix);
    
    if (rank == 0) {
        free(sendcounts);
        free(displs);
    }

    GET_TIME(overall_end);

    if (rank == 0) {
        printf("Total time: %lf seconds\n", overall_end - overall_start);
        printf("Work time: %lf seconds\n", work_end - work_start);
    }

    MPI_Finalize();
    return 0;
}

void mpi_apply_stencil_local(double *input, double *output, int local_rows, int cols, int rank, int size) {
    MPI_Request requests[4];
    int top = rank - 1;
    int bottom = rank + 1;

    if (top >= 0) {
        MPI_Irecv(input, cols, MPI_DOUBLE, top, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Isend(&input[cols], cols, MPI_DOUBLE, top, 1, MPI_COMM_WORLD, &requests[1]);
    }
    if (bottom < size) {
        MPI_Irecv(&input[(local_rows + 1) * cols], cols, MPI_DOUBLE, bottom, 1, MPI_COMM_WORLD, &requests[2]);
        MPI_Isend(&input[local_rows * cols], cols, MPI_DOUBLE, bottom, 0, MPI_COMM_WORLD, &requests[3]);
    }

    if (top >= 0) MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    if (bottom < size) MPI_Waitall(2, requests + 2, MPI_STATUSES_IGNORE);

    for (int i = 1; i <= local_rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i == 1 && rank == 0) {
                output[i * cols + j] = input[i * cols + j];
                continue;
            }
            if (i == local_rows && rank == size - 1) {
                output[i * cols + j] = input[i * cols + j];
                continue;
            }
            if (j == 0 || j == cols - 1) {
                output[i * cols + j] = input[i * cols + j];
                continue;
            }

            double NW = input[(i-1) * cols + (j-1)];
            double N  = input[(i-1) * cols + j];
            double NE = input[(i-1) * cols + (j+1)];
            double E  = input[i * cols + (j+1)];
            double SE = input[(i+1) * cols + (j+1)];
            double S  = input[(i+1) * cols + j];
            double SW = input[(i+1) * cols + (j-1)];
            double W  = input[i * cols + (j-1)];
            double C  = input[i * cols + j];

            output[i * cols + j] = (NW + N + NE + E + SE + S + SW + W + C) / 9.0;
        }
    }
}
