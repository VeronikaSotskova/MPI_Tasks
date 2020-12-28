#include "task6.h"

// седловая точка матрицы
void task6(int lines, int columns, int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int **arr;
    int max_of_min;
    int min_of_max;
    arr = new int *[lines];

    int partition = (int) round((double) lines / size);

    if(lines % size != 0) {
        if (rank == size - 1) partition = lines - partition * (size-1);
    }

    int *displs = new int[size];
    int *sendcounts = new int[size];
    int *send_arr = new int[lines * columns];

    int *buf = new int[partition * columns];
// генерируем матрицу, и представляем ее в виде массива
    if (rank == 0) {

        for (int i = 0; i < lines; i++) {
            arr[i] = new int[columns];
            for (int j = 0; j < columns; j++) {
                arr[i][j] = rand() % 10;
                printf("%d ", arr[i][j]);
                send_arr[columns * i + j] = arr[i][j];
            }
            printf("\n");
        }

        printf("\n");

        for (int i = 0; i < size; i++) {
            displs[i] = i * partition * columns;
            sendcounts[i] = partition * columns;
        }

        if(lines % size != 0) {
            sendcounts[size-1] = (lines - partition * (size-1)) * columns;
        }
    }
// послыаем каждому процессу часть массива
    MPI_Scatterv(send_arr, sendcounts, displs, MPI_INT, buf, partition * columns, MPI_INT, 0, MPI_COMM_WORLD);

    int local_ans_max = -1;
    int local_ans_min = 12;

    // проходим по каждой строке
    for (int i = 0; i < partition; ++i) {
        int local_min = buf[columns * i];
        int local_max = buf[columns * i];
        // проходим по каждому элементу в строке; ищем в каждой строке минимум
        for (int j = 0; j < columns; ++j) {
            // ищем в каждой строке минимум
            if (buf[columns * i + j] < local_min) local_min = buf[columns * i + j];
            // ищем в каждой строке максимум
            if (buf[columns * i + j] > local_max) local_max = buf[columns * i + j];
        }
        // максимум среди минимумов
        if (local_ans_max < local_min) local_ans_max = local_min;
        // миниммум среди максимумов
        if (local_ans_min > local_max) local_ans_min = local_max;
    }

    MPI_Reduce(&local_ans_max, &max_of_min, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_ans_min, &min_of_max, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Maximum of min: %d \n",max_of_min);
        printf("Minimum of max: %d \n",min_of_max);
        if (max_of_min == min_of_max) {
            printf("MaxMin = MinMax");
        } else {
            printf("MaxMin != MinMax");
        }
    }

    MPI_Finalize();
}