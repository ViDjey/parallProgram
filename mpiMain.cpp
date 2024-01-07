#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
    int n = 1000;
    int rank, size, threads_num;
    double time;
    double *A = new double[n * n];
    double *B = new double[n * n];
    double *result = new double[n * n];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    int additional = 0, amount = n / size;
    if (rank == size - 1 && n % size != 0) additional = n % size;
    ifstream file1, file2;
    if (rank == 0) {
        file1.open("Matrix1.txt");
        file2.open("Matrix2.txt");
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                file1 >> A[i * n + j];
                file2 >> B[i * n + j];
            }
        file1.close(); file2.close();
        time = MPI_Wtime();
        threads_num = 1;
        while (threads_num < size) {
            MPI_Send(&A[threads_num * amount * n], (threads_num == size - 1) ? (amount + n % size) * n : amount * n, MPI_DOUBLE,
                     threads_num, 1, MPI_COMM_WORLD);
            MPI_Send(&B[0], n * n, MPI_DOUBLE, threads_num, 2, MPI_COMM_WORLD);
            ++threads_num;
        }
    } else {
        MPI_Recv(&A[rank * amount * n], (amount + surplus) * n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&B[0], n * n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
    }
    for (int i = rank * amount; i < amount * (rank + 1) + additional; i++) {
        for (int j = 0; j < n; j++) {
            result[i * n + j] = 0;
            for (int t = 0; t < n; t++) {
                result[i * n + j] += A[i * n + t] * B[t * n + j];
            }
        }
    }

    delete[] A;
    delete[] B;

    if (rank != 0) MPI_Send(&result[rank * amount * n], (amount + surplus) * n, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
    else {
        threads_num = 1;
        while (threads_num < size) {
            MPI_Recv(&result[threads_num * amount * n], (threads_num == size - 1) ? (amount + n % size) * n : amount * n, MPI_DOUBLE,
                     threads_num, 4, MPI_COMM_WORLD, &status);
            ++threads_num;
        }
        cout << "Time: " << MPI_Wtime() - time << endl;
    }

    MPI_Finalize();
    delete[] result;
    return 0;
}