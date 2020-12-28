#include "task12.h"

// передача числа по кругу, внутри другого коммуникатора
void task12(int num, int argc, char **argv) {
    int size, rank;

    MPI_Init(&argc, &argv);

    MPI_Group MPI_GROUP_WORLD, even_gr;
    MPI_Comm even_comm;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
    int group_count = size / 2;

    // выбираем четные ранки для новой группы
    int *ranks = new int[group_count];
    for (int i = 0; i < group_count; ++i) {
        ranks[i] = i * 2;
    }

    // создаем новую группу из четных ранков основного коммуникатора
    MPI_Group_incl(MPI_GROUP_WORLD, group_count, ranks, &even_gr);

    // создаем коммуникатор для четной группы
    MPI_Comm_create(MPI_COMM_WORLD, even_gr, &even_comm);

    if (even_comm != MPI_COMM_NULL) {
        // передаем число внутри четного коммуникатора
        Passing_num_in_circle(num, even_comm);
    }


    MPI_Finalize();
}