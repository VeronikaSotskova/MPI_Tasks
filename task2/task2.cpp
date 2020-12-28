#include "task2.h"

// максимум в массиве
void task2(int length, int argc, char **argv) {
    int rank, size;
    int local_max = 0, max = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int partition = length / size;

    int *arr = new int[length];
    int *receive_arr = new int[partition];

// если рут процесс
    if (rank == 0) {

        //генерируем массив
        for (int i = 0; i < length; i++) {
            arr[i] = rand();
            printf("%d ", arr[i]);
        }
        printf("\n");

    }
    //отправляем часть массива каждому процессу
    MPI_Scatter(arr, partition, MPI_INT, receive_arr, partition, MPI_INT, 0, MPI_COMM_WORLD);
    // Ищем локальный максимум на каждом процессе
    for (int i = 0; i < partition; i++) {
        if (local_max < receive_arr[i]) local_max = receive_arr[i];
    }

    // собираем результат
    MPI_Reduce(&local_max, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MAX: %d", max);
    }
    MPI_Finalize();
}