#include "task7.h"

// Умножение матрицы на вектор при разделении данных по столбцам
void task7(int lines, int columns, int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int **arr;
    arr = new int *[lines];
    int *vector = new int[columns];

    int partition = (int) round((double) columns / size);

    if (columns % size != 0) {
        if (rank == size - 1) partition = columns - (size - 1) * partition;
    }
    int *send_arr = new int[columns * lines];

    int *buf_arr = new int[partition * lines];
    int *buf_vector = new int[partition];


    int *displs = new int[size];
    int *sendcounts = new int[size];

    int *displs_vec = new int[size];
    int *sendcounts_vec = new int[size];



    // заполняем матрицу и вектор
    if (rank == 0) {
        for (int i = 0; i < lines; i++) {
            arr[i] = new int[columns];
            for (int j = 0; j < columns; ++j) {
                arr[i][j] = rand() % 10;
                send_arr[lines * j + i] = arr[i][j];
            }
        }

        for (int i = 0; i < columns; i++) {
            vector[i] = rand() % 10;
        }

        for (int i = 0; i < size; i++) {
            displs[i] = i * partition * lines;
            sendcounts[i] = partition * lines;
            displs_vec[i] = i * partition;
            sendcounts_vec[i] = partition;
        }

        if (columns % size != 0) {
            sendcounts[size - 1] = (columns - (size - 1) * partition) * lines;
            sendcounts_vec[size - 1] = columns - (size - 1) * partition;
        }

    }

    // отправляем столбцы матрицы
    MPI_Scatterv(send_arr, sendcounts, displs, MPI_INT, buf_arr, partition * lines, MPI_INT, 0, MPI_COMM_WORLD);
    // отправляем часть вектора
    MPI_Scatterv(vector, sendcounts_vec, displs_vec, MPI_INT, buf_vector, partition, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);


    int *part_sum_vec = new int[lines];
    int *res_sum_vec = new int[lines];
    for (int i = 0; i < lines; ++i) {
        part_sum_vec[i] = 0;
    }

    // считаем часть результирцющего вектора
    for (int i = 0; i < lines; ++i) {
        for (int j = 0; j < partition; ++j) {
            part_sum_vec[i] += buf_vector[j] * buf_arr[lines * j + i];
        }
    }

    // собираем сумму
    MPI_Reduce(part_sum_vec, res_sum_vec, lines, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < lines; i++) {
            for (int j = 0; j < columns; ++j) {
                printf("%d ", arr[i][j]);
            }
            printf("\n");
        }

        printf("\n");

        for (int j = 0; j < columns; ++j) {
            printf("%d ", vector[j]);
        }

        printf("\n\n");

        for (int i = 0; i < lines; ++i) {
            printf("%d \n", res_sum_vec[i]);
        }

    }

    MPI_Finalize();
}