#include "task9.h"

// Перевернуть массив
void task9(int arrLength, int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int partition =(int) round( (double)arrLength / size);

    if(arrLength % size != 0) {
        if (rank == size - 1) partition = arrLength - partition * (size-1);
    }

    int *arr = new int[arrLength];
    int *reversed_array = new int[arrLength];
    int *displs = new int[size];

    // сдвиг при приеме на 0 процессе
    int *recv_displs = new int[size];
    int *sendcounts = new int[size];

    int *buf = new int[partition];

    if (rank == 0) {
        printf("Array: \n");
        for (int i = 0; i < arrLength; ++i) {
            arr[i] = rand() % 1000;
            printf("%d ", arr[i]);
        }
        printf("\n");

        for (int i = 0; i < size; ++i) {
            displs[i] = i * partition;
            sendcounts[i] = partition;

            //recv_displs[i] = partition * (size - i - 1);
        }

        if (arrLength % size != 0) {
            sendcounts[size-1] = arrLength - partition * (size-1);
        }
        recv_displs[size-1] = 0;
        for (int i = size - 2; i >= 0; i--) {
            recv_displs[i] = recv_displs[i+1] + sendcounts[i+1];
        }
    }

    // Отправляем часть массива каждому процессу
    MPI_Scatterv(arr, sendcounts, displs, MPI_INT, buf, partition, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // каждый процесс переворачивает свою часть массива
    for (int i = 0; i < partition / 2; ++i) {
        int temp = buf[i];
        buf[i] = buf[partition - i - 1];
        buf[partition - i - 1] = temp;
    }

    // каждый процесс отправляет 0 процессу в обратном порядке
    MPI_Gatherv(buf, partition, MPI_INT, reversed_array, sendcounts, recv_displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Reversed Array: \n");
        for (int i = 0; i < arrLength; ++i) {
            printf("%d ", reversed_array[i]);
        }
    }

    MPI_Finalize();
}