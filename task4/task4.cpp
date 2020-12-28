#include "task4.h"

// Среднее арифметическое среди положительных чисел массива
void task4(int arrLength, int argc, char **argv) {
    int count = 0, sum = 0;
    long int result[2];

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int partition =(int) round( (double)arrLength / size);

    if(arrLength % size != 0) {
        if (rank == size - 1) partition = arrLength - partition * (size-1);
    }

    int *arr = new int[arrLength];
    int *slave_buf = new int[partition];


    // сдвиг для каждого процесса
    int *displs = new int[size];
    // количество посылаемых элементов для каждого процесса
    int *sendcounts = new int[size];

    // на 0 процессе генерируем массив
    if (rank == 0) {
        printf("Elements: \n");
        for (int i = 0; i < arrLength; i++) {
            arr[i] = (rand() % 100) * pow(-1.0, rand());
            printf("%d ", arr[i]);
        }
        printf("\n");

        for (int i = 0; i < size; ++i) {
            displs[i] = i * partition;
            sendcounts[i] = partition;
        }

        if (arrLength % size != 0) {
            sendcounts[size-1] = arrLength - partition * (size-1);
        }
    }
// отправляем свою часть массива каждому процессу
    MPI_Scatterv(arr, sendcounts, displs, MPI_INT, slave_buf, partition, MPI_INT, 0, MPI_COMM_WORLD);

    int slave_ans[2];

    for (int i = 0; i < partition; i++) {
        printf("%d ", slave_buf[i]);
        if (slave_buf[i] > 0) {
            count++;
            sum += slave_buf[i];
        }
    }


    printf("\n");
    printf("rank: %d, sum: %d, count: %d\n", rank, sum, count);

    slave_ans[0] = count;
    slave_ans[1] = sum;

// сливаем результат в 0 процесс
    MPI_Reduce(slave_ans, result, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double answer = (double) result[1] / (double) result[0];
        printf("avg is %f, sum = %d; count = %d", answer, result[1], result[0]);
    }

    MPI_Finalize();
}