#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define FILAS 10000 //Tanto en FILAS como COLUMNAS mi pc soporta hasta 10000
#define COLUMNAS 10000//Pues luego de las pruebas el código se ejecuta sin problemas hasta tal valor.

int main(int argc, char** argv) {
    int i, j, rank, size;
    int** local_matrix;
    int local_rows, local_cols, count, total_count = 0;
    int num_busq;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf("Uso: mpirun -n <numero_de_procesos> %s <numero_a_buscar>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    num_busq = atoi(argv[1]);

    // Calcular la carga de trabajo local
    local_rows = FILAS / size;
    local_cols = COLUMNAS;
    local_matrix = malloc(local_rows * sizeof(int*));
    for (i = 0; i < local_rows; i++) {
        local_matrix[i] = malloc(local_cols * sizeof(int));
    }

    // Generar la matriz local en todos los procesos
    srand(time(NULL) + rank);
    for (i = 0; i < local_rows; i++)
    {
        for (j = 0; j < local_cols; j++)
        {
            local_matrix[i][j] = rand() % 100;
        }
    }

    // Busqueda del número de interés en la matriz local
    count = 0;
    for (i = 0; i < local_rows; i++) {
        for (j = 0; j < local_cols; j++) {
            if (local_matrix[i][j] == num_busq) {
                count++;
            }
        }
    }

    // Recopilar el resultado parcial de cada proceso en el proceso 0
    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Imprimir el resultado final en el proceso 0
    if (rank == 0) {
        printf("El número %d aparece %d veces en la matriz.\n", num_busq, total_count);
    }

    // Liberar la memoria asignada a la matriz local
    for (i = 0; i < local_rows; i++) {
        free(local_matrix[i]);
    }
    free(local_matrix);

    MPI_Finalize();
    return 0;
}

