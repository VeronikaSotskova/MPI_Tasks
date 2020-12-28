#include "task13.h"

// Проверка матрицы на симметричность
void task13(int n, int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int **arr = new int *[n];
    int length_send_arr = n * n - n;
    int *send_arr = new int[length_send_arr];

    int partition = (int) round((double) length_send_arr / (size * 2));

    if (length_send_arr % (size*2) != 0) {
        if (rank == size - 1) partition = (length_send_arr / 2) - (size - 1) * partition;
    }

    int *buf_arr = new int[partition*2];
    int *displs = new int[size];
    int *sendcounts = new int[size];

    //кол-во совпавших пар [ij] [ji] элементов
    int count = 0;

    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            arr[i] = new int[n];
            for (int j = 0; j < n; ++j) {
                arr[i][j] = i + j;
                printf("%d ", arr[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        int index = 0;
        // представляем матрицу, чтобы рядом стояли элементы [ij] [ji]
        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < i; ++j) {
                send_arr[index] = arr[i][j];
                send_arr[index + 1] = arr[j][i];
                index += 2;
            }
        }

        for (int i = 0; i < size; ++i) {
            displs[i] = i * partition * 2;
            sendcounts[i] = partition * 2;
        }
        if (length_send_arr % (size * 2) != 0) {
            sendcounts[size-1] = ((length_send_arr / 2) - (size - 1) * partition) * 2;
        }
    }

    MPI_Scatterv(send_arr, sendcounts, displs, MPI_INT, buf_arr, partition*2, MPI_INT, 0, MPI_COMM_WORLD);

    int local_count = 0;

    // на каждом процессе считаем кол-во равных элементов
    for (int i = 0; i < partition*2 - 1; i += 2) {
        printf("rank: %d, %d %d\n", rank, buf_arr[i], buf_arr[i+1]);
        if (buf_arr[i] == buf_arr[i + 1]) {
            local_count++;
        }
    }
    printf("\n");

    MPI_Reduce(&local_count, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (count == length_send_arr / 2) {
            printf("Matrix is symmetric");
        } else {
            printf("Matrix not symmetric");
        }
    }

    MPI_Finalize();
}