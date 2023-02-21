#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10000

int main(int argc, char** argv) {
    int rank, size, i, j;
    double start_time, end_time;
    double sum = 0.0, max_val = 0.0;
    int max_index = 0;
    double *matrix = (double*)malloc(N*N*sizeof(double));
    double *diagonal = (double*)malloc(N*sizeof(double));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    start_time = MPI_Wtime();

    // LLena la matriz con valores aleatorios
    for (i = rank; i < N*N; i += size) {
        matrix[i] = rand() / (double)RAND_MAX;
    }

    // Extraer los elementos de la diagonal
    for (i = rank; i < N; i += size) {
        diagonal[i] = matrix[i*N + i];
    }

    // Suma los elementos de la diagonal
    for (i = 0; i < N; i++) {
        if (i % size == rank) {
            sum += diagonal[i];
        }
    }

    // Encuentra el elemento máximo en la diagonal
    for (i = 0; i < N; i++) {
        if (i % size == rank) {
            if (diagonal[i] > max_val) {
                max_val = diagonal[i];
                max_index = i;
            }
        }
    }

    //Determinar qué proceso tiene el elemento diagonal máximo
    int max_rank = 0;
    double max_local = max_val;
    MPI_Allreduce(&max_local, &max_val, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    if (max_local == max_val) {
        max_rank = rank;
    }
    MPI_Bcast(&max_rank, 1, MPI_INT, max_rank, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    // Imprimir el resultado
    if (rank == 0) {
        printf("Tiempo empleado: %f segundos\n", end_time - start_time);
        printf("Suma de los elementos en la diagonal: %f\n", sum);
        printf("Elemento en la diagonal más grande]: %f (process %d, index %d)\n", max_val, max_rank, max_index);
    }

    free(matrix);
    free(diagonal);

    MPI_Finalize();

    return 0;
}
