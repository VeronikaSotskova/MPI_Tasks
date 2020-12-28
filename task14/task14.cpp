#include "task14.h"

// Сортировка массива методом чет-нечетной перестановки
void task14(int n, int argc, char **argv) {
    int size, rank;
    int *data;
    int *temp;
    int i;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int partition = n / size;

    int *recv_data=(int*)malloc(sizeof(int)*partition);
    int *recv_data2=(int*)malloc(sizeof(int)*partition);

    if (rank == 0) {
        data=(int*)malloc(sizeof(int)*n);
        printf("array data is:");
        for (i = 0; i < n; i++) {
            data[i] = rand() % 100;
            printf("%d ", data[i]);
        }
        printf("\n");
    } else {
        data = NULL;
    }
    MPI_Scatter(data, partition, MPI_INT, recv_data, partition, MPI_INT, 0, MPI_COMM_WORLD);
    printf("%d:received data:", rank);
    for (i = 0; i < partition; i++) {
        printf("%d ", recv_data[i]);
    }
    printf("\n");
    std::sort(recv_data, recv_data + partition);

    //begin the odd-even sort
    int oddrank, evenrank;

    if (rank % 2 == 0) {
        oddrank = rank - 1;
        evenrank = rank + 1;
    } else {
        oddrank = rank + 1;
        evenrank = rank - 1;
    }
/* Set the ranks of the processors at the end of the linear */
    if (oddrank == -1 || oddrank == size)
        oddrank = MPI_PROC_NULL;
    if (evenrank == -1 || evenrank == size)
        evenrank = MPI_PROC_NULL;

    int p;
    for (p = 0; p < size - 1; p++) {
        if (p % 2 == 1) /* Odd phase */
            MPI_Sendrecv(recv_data, partition, MPI_INT, oddrank, 1, recv_data2,
                         partition, MPI_INT, oddrank, 1, MPI_COMM_WORLD, &status);
        else /* Even phase */
            MPI_Sendrecv(recv_data, partition, MPI_INT, evenrank, 1, recv_data2,
                         partition, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &status);

        //extract partition after sorting the two
        temp = (int *) malloc(partition * sizeof(int));
        for (i = 0; i < partition; i++) {
            temp[i] = recv_data[i];
        }
        if (status.MPI_SOURCE == MPI_PROC_NULL) continue;
            // if from right neighbor
        else if (rank < status.MPI_SOURCE) {
            //store the smaller of the two
            int i, j, k;
            for (i = j = k = 0; k < partition; k++) {
                if (j == partition || (i < partition && temp[i] < recv_data2[j]))
                    recv_data[k] = temp[i++];
                else
                    recv_data[k] = recv_data2[j++];
            }
        } else {
            //store the larger of the two
            int i, j, k;
            for (i = j = k = partition - 1; k >= 0; k--) {
                if (j == -1 || (i >= 0 && temp[i] >= recv_data2[j]))
                    recv_data[k] = temp[i--];
                else
                    recv_data[k] = recv_data2[j--];
            }
        }//else
    }//for



    MPI_Gather(recv_data, partition, MPI_INT, data, partition, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("final sorted data:");
        for (i = 0; i < n; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

}