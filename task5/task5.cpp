#include "task5.h"

// Скалярное произведение
void task5(int arrLength, int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //кол-во элементов для последнего процесса

    int partition =(int) round( (double)arrLength / size);

    if(arrLength % size != 0) {
        if (rank == size - 1) partition = arrLength - partition * (size-1);
    }

    int *arr_1 = new int[arrLength];
    int *arr_2 = new int[arrLength];
    int *displs = new int[size];
    int *sendcounts = new int[size];
    int *buf_arr_1 = new int[partition];
    int *buf_arr_2 = new int[partition];

    // на руте генерируем массивы
    if (rank == 0) {

        for (int i = 0; i < arrLength; i++) {
            arr_1[i] = rand() % 10;
            arr_2[i] = rand() % 10;
            printf("%d %d\n", arr_1[i], arr_2[i]);
        }

        for (int i = 0; i < size; ++i) {
            displs[i] = i * partition;
            sendcounts[i] = partition;
        }

        if (arrLength % size != 0) {
            sendcounts[size-1] = arrLength - partition * (size-1);
        }
    }

    // пересылаем каждому процессу его часть массивов
    MPI_Scatterv(arr_1, sendcounts, displs, MPI_INT, buf_arr_1, partition, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(arr_2, sendcounts, displs, MPI_INT, buf_arr_2, partition, MPI_INT, 0, MPI_COMM_WORLD);


    long long int part_sum = 0;
    long long int sum = 0;

    // считаем сумму произведений между частями двух массивов
    for (int i = 0; i < partition; i++) {
        part_sum += (long int) (buf_arr_1[i] * buf_arr_2[i]);
        printf("rank: %d, %d %d\n", rank,buf_arr_1[i], buf_arr_2[i]);
    }


    // собираем результат
    MPI_Reduce(&part_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("scalar multiple: %lld", sum);
    }

    MPI_Finalize();
}