#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

int main()
{
    int n = 1000;
    int threads_num;
    double** A = NULL, ** B = NULL, ** result = NULL;
    ifstream file1, file2;

    threads_num = 1;
    cout << "Processes: " << threads_num << endl;


    omp_set_num_threads(threads_num);

    A = new double* [n];
    B = new double* [n];
    result = new double* [n];

    for (int i = 0; i < n; i++){
        A[i] = new double[n];
        B[i] = new double[n];
        result[i] = new double[n];
    }

    file1.open("Matrix1.txt");
    file2.open("Matrix2.txt");
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            file1 >> A[i][j];
            file2 >> B[i][j];
        }
    }
    file1.close();
    file2.close();

    double calc_time = omp_get_wtime();

    int i, j, k;

#pragma omp parallel num_threads(threads_num)
    {
#pragma omp for private (i,j,k)
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                result[i][j] = 0;
                for (k = 0; k < n; k++) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }


    calc_time = omp_get_wtime() - calc_time;
    cout << "Time: " << calc_time << endl;

//    for (int i = 0; i < n; i++){
//        for (int j = 0; j < n; j++){
//            cout << C[i][j] << " ";
//        }
//        cout << endl;
//    }
    system("pause");
    return 0;
}
