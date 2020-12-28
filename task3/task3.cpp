#include "task3.h"

// подсчет числа Пи методом Монте Карло количество точек
void task3(int count, int argc, char **argv) {
    int rank, size;
    double x, y;

    int dots_count = 0, local_dots_count = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int partition = count / size;

    // каждый проуесс генерирует координаты точек
    for (int i = 0; i < partition; ++i) {
        x = (double) rand() / (double) RAND_MAX * 2 - 1;
        y = (double) rand() / (double) RAND_MAX * 2 - 1;

        // количество точек, которые оказались внутри круга
        if (pow(x, 2) + pow(y, 2) <= 1)
            local_dots_count++;
    }

    // суммируем общее количество точек, которые попали в круг на 0 процессе
    MPI_Reduce(&local_dots_count, &dots_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double answer = (double) (4 * dots_count) / (double) count;
        printf("Pi = %f    %d", answer, dots_count);
    }
    MPI_Finalize();
}