#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define M 1000
#define N 500
#define p 65535 //(2^16-1)
#define q 4294967296 // (2^32)...



using namespace std;

// Main key-homomorphic PRF
long long int* prf(long long int A[N][M], long long int x[M]);

// Initialize random arrays (not using files)
void initializeArrays(double A[N][M], double x[M]);

// Initialize random arrays into files
void initializeArraysIntoFiles();

// A is stored in a file while we use x from stack. Worse case, might have to 
// make result vector into file... will see
double* prf_from_files(double x[M]);

int main(int argc, char *argv[]){
    // long long int A[N][M] = {};
    //long long int x[M] = {};

    double x[M] = {};

    // initializeArrays(A, x);
    // cout << "Left initalize arrays" << endl;
    // long long int* randomNum = prf(A, x);

    // ofstream myfile;
    // myfile.open("output.txt");
    // for(int i = 0; i < N; i++){
    //     cout << randomNum[i] << " ";
    // }
    // myfile.close();

    // return 0;

    // initialize x and A
    initializeArraysIntoFiles();

    // we can read into x (I think) since it shouldn't break
    int i = 0;
    long long int value;
    ifstream x_file;
    x_file.open("x.txt");
    while(x_file >> value){
        x[i] = value;
        i++;
    }
    x_file.close();

    double* randomNum = prf_from_files(x);
    for(int i = 0; i < N; i++){
        cout << randomNum[i] << " ";
    }

    return 0;
}

void initializeArraysIntoFiles(){
    random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<long long int> distr(0, q);
    cout << "Initializing arrays and placing into files" << endl;
    ofstream A_file, x_file;
    A_file.open("A.txt");
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            A_file << distr(gen) << " ";
        }

        A_file << endl;
    }

    A_file.close();

    x_file.open("x.txt");
    for(int i = 0; i < M; i++){
        x_file << distr(gen) << " ";
    }

    x_file.close();
    return;
}

void initializeArrays(long long int A[N][M], long long int x[M]){
    random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<long long int> distr(0, p-1);
    cout << "in Initialize Arrays" << endl;
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            A[i][j] = distr(gen);
        }
    }

    for(int i = 0; i < M; i++){
        x[i] = distr(gen);
    }
}

double* prf_from_files(double x[M]){
    static double rand[N];
    double ratio = q / p;

    // double temp_product = 0;
    double temp[M] = {};

    string line;
    ifstream A_file;
    int count = 0;
    int linecount = 0;
    A_file.open("A.txt");
    while(A_file){
        getline(A_file, line);
        stringstream stream(line);
        
        long long int i;

        while(stream >> i){
            temp[count] = i;
            count++;
        }
        count = 0;


        // do the calculation you need for this line
        for(int i = 0; i < M; i++){
            rand[linecount] += (temp[i] * x[linecount]);
        }

        cout << "Start rounding" << endl;
        // rounding protocol
        double count2 = 1;
        while(count2 * ratio < rand[linecount]){
            count2 *= 2;
        }

        rand[linecount] = round((count2-1) * ratio);
        count2 = 1;

        linecount++;
        //count++;
        cout << "end rounding" << endl;
    }

    return rand;
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