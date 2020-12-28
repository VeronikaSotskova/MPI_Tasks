#include "task8.h"

// Scatter и Gather через Send и Recv
void task8(int arrLength, int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int partition = arrLength / size;
    int *arr = new int[arrLength];
    int *buf = new int[partition];
    int *result_array = new int[arrLength];

    if (rank == 0) {

        for (int i = 0; i < arrLength; ++i) {
            arr[i] = rand() % 1000;
            printf("%d ", arr[i]);
        }
        printf("\n\n");

        // с 0 процесса отправляем всем остальным
        for (int i = 0; i < size; ++i) {
            int *send_buf = new int[partition];
            for (int j = 0; j < partition; ++j) {
                send_buf[j] = arr[i * partition + j];
            }
            if (i == 0) {
                buf = send_buf;
            } else {
                MPI_Send(send_buf, partition, MPI_INT, i, i + 10, MPI_COMM_WORLD);
            }
        }
    }

    // на всех остальных процессаах получаем
    if (rank != 0) {
        MPI_Recv(buf, partition, MPI_INT, 0, rank + 10, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    }

    printf("rank = %d\n", rank);
    for (int i = 0; i < partition; ++i) {
        printf("%d ", buf[i]);
    }
    printf("\n\n");

    if (rank != 0) {
        // со всех процессов отправляем 0 свою часть
        MPI_Send(buf, partition, MPI_INT, 0, rank + 100, MPI_COMM_WORLD);
    } else {
        // заполняем на 0 процессе результирующий массив из буфера 0 процесса
        for (int i = 0; i < partition; ++i) {
            result_array[i] = buf[i];
        }
    }

    if (rank == 0) {
        // получаем данные и заполняем ими массив
        for (int i = 1; i < size; ++i) {
            MPI_Recv(buf, partition, MPI_INT, i, i + 100, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            for (int j = 0; j < partition; ++j) {
                result_array[i*partition+j] = buf[j];
            }
        }

        printf("Answer: ");
        for (int i = 0; i < arrLength; ++i) {
            printf("%d ", result_array[i]);
        }
    }

    MPI_Finalize();
}