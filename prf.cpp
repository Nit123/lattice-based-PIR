#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define M 1000
#define N 500
#define p 65535 //(2^16-1)
#define q 4294967296


using namespace std;

// Main key-homomorphic PRF
long long int* prf(long long int A[N][M], long long int x[M]);

// Initialize random arrays
void initializeArrays(long long int A[N][M], long long int x[M]);

int main(int argc, char *argv[]){
    long long int A[N][M] = {};
    long long int x[M] = {};

    cout << "Here?" << endl;

    initializeArrays(A, x);
    cout << "Left initalize arrays" << endl;
    long long int* randomNum = prf(A, x);

    ofstream myfile;
    myfile.open("output.txt");
    for(int i = 0; i < N; i++){
        cout << randomNum[i] << " ";
    }
    myfile.close();

    return 0;
}


void initializeArrays(long long int A[N][M], long long int x[M]){
    random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<long long int> distr(0, p-1);
    cout << "in Initalize Arrays" << endl;
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            A[i][j] = distr(gen);
        }
    }

    for(int i = 0; i < M; i++){
        x[i] = distr(gen);
    }
}

long long int* prf(long long int A[N][M], long long int x[M]){
    cout << "in PRF" << endl;
    static long long int rand[N];
    double ratio = q / p;

    double temp = 0;

    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            temp += (A[i][j] * x[j]);
        }

        // // // rounding to p
        //     // // dumb way: x = rand[i]
        //     // // ratio while loop until we get the answer
            int count = 1;

            cout << "rounding" << endl;
            while(count * ratio < temp){
                count++;
            }
            cout << "rounding done." << endl;

            if(count - 1 == 0){
                cout << "ratio: " << ratio << "temp: " << temp << endl; 
            }
            rand[i] = (count - 1) * ratio;
           
            count = 1;
            temp = 0;
        // }
    }

    return rand;
}