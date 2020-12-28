#include "task11.h"

// передача чисел по кругу внутри коммуникатора
void Passing_num_in_circle(int num, MPI_Comm comm) {
    int rank1, size1;

    MPI_Comm_rank(comm, &rank1);
    MPI_Comm_size(comm, &size1);

    int *send = new int[1];
    int *recv = new int[1];

    int to = rank1 + 1;
    int from = rank1 - 1;

    if (rank1 == 0) {
        from = size1 - 1;
    }

    if (rank1 == size1 - 1) {
        to = 0;
    }

    // 0 процесс отправляет следующему процессу число
    if (rank1 == 0) {
        send[0] = num;
        MPI_Send(send, 1, MPI_INT, to, to, comm);
        printf("Send from %d to %d: %d\n", rank1, to, send[0]);
    }
    if (rank1 != 0) {
        // каждый процесс получает число, умножает на 2 и отправляет следующему
        MPI_Recv(recv, 1, MPI_INT, from, rank1, comm, MPI_STATUSES_IGNORE);
        printf("Receive from %d to %d: %d\n", from, rank1, recv[0]);
        send[0] = recv[0] * 2;

        MPI_Send(send, 1, MPI_INT, to, to, comm);
        printf("Send from %d to %d: %d\n", rank1, to, send[0]);
    }

    if (rank1 == 0) {
        // 0 процесс получает число от последнего
        MPI_Recv(recv, 1, MPI_INT, from, 0, comm, MPI_STATUSES_IGNORE);
        printf("Receive from %d to %d\nResult: %d\n", from, rank1, recv[0]);
    }


}

// Циклическая передача данных
void task11(int num, int argc, char **argv) {
    MPI_Init(&argc, &argv);
    Passing_num_in_circle(num, MPI_COMM_WORLD);
    MPI_Finalize();
}