#include "task10.h"

// Время передачи для разных Send-oв
void task10(int arrLength, int argc, char **argv) {
    int message_buffer_size = arrLength * sizeof(int) + MPI_BSEND_OVERHEAD;
    int rank, size;
    int *sbuf = new int[arrLength];
    int *rbuf = new int[arrLength];
    int *buf = (int *) malloc(message_buffer_size);
    double t1, t2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        for (int i = 0; i < arrLength; ++i) {
            sbuf[i] = rand() % 10000;
        }
    }

    if (rank == 0) {
        t1 = MPI_Wtime();
        MPI_Send(sbuf, arrLength, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Recv(rbuf, arrLength, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        t2 = MPI_Wtime();
        printf("Array Length: %d\nTime Send: %f \n", arrLength, t2 - t1);


        // функция отправки сообщения в синхронном режиме
        t1 = MPI_Wtime();
        MPI_Ssend(sbuf, arrLength, MPI_INT, 1, 3, MPI_COMM_WORLD);
        MPI_Recv(rbuf, arrLength, MPI_INT, 1, 3, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        t2 = MPI_Wtime();
        printf("Time Ssend: %f \n", t2 - t1);
        MPI_Barrier(MPI_COMM_WORLD);

        // функция отправки сообщения в буферизованном режиме,
        t1 = MPI_Wtime();
        //буфер памяти для буферизации сообщений
        MPI_Buffer_attach(buf, message_buffer_size);
        MPI_Bsend(sbuf, arrLength, MPI_INT, 1, 4, MPI_COMM_WORLD);
        MPI_Recv(rbuf, arrLength, MPI_INT, 1, 4, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        t2 = MPI_Wtime();
        printf("Time Bsend: %f \n", t2 - t1);
        MPI_Buffer_detach(buf, &message_buffer_size);
        free(buf);

        // функция отправки сообщения в режиме по готовности
        t1 = MPI_Wtime();
        MPI_Rsend(sbuf, arrLength, MPI_INT, 1, 5, MPI_COMM_WORLD);
        MPI_Recv(rbuf, arrLength, MPI_INT, 1, 5, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        t2 = MPI_Wtime();
        printf("Time Rsend: %f \n", t2 - t1);


    } else {
        MPI_Recv(rbuf, arrLength, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Send(sbuf, arrLength, MPI_INT, 0, 2, MPI_COMM_WORLD);

        MPI_Recv(rbuf, arrLength, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Ssend(sbuf, arrLength, MPI_INT, 0, 3, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(rbuf, arrLength, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Buffer_attach(buf, message_buffer_size);
        MPI_Bsend(sbuf, arrLength, MPI_INT, 0, 4, MPI_COMM_WORLD);
        MPI_Buffer_detach(buf, &message_buffer_size);
        free(buf);

        MPI_Recv(rbuf, arrLength, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Rsend(sbuf, arrLength, MPI_INT, 0, 5, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}