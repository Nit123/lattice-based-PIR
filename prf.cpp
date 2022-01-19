#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


#define M 1000 // change to 500 for long long int version
#define N 500 // change to 250 for long long int version
#define p 65535 //(2^16-1)
#define q 4294967296 // (2^32)...
#define ratio q/p



using namespace std;

/*
    INTEGER VERSION OF THE PRF: requires M and N to be smaller 
*/
long long int* prf(long long int A[N][M], long long int x[M]); // main PRF function
void initializeArrays(long long int A[N][M], long long int x[M]); // initializes random arrays
void testIntegerPRF(); // tester


/*
    FLOATING POINT VERSION OF PRF: M and N can be large but requires matrices to be outputted and read to a file
*/
void initializeArraysIntoFiles(); // Initialize random arrays into files
double* prf_from_files(); // PRF that reads from files
void testRealPRF(); // tester

int main(int argc, char *argv[]){
    //testIntegerPRF();
    testRealPRF();

    return 0;
}

void testIntegerPRF(){
    long long int A[N][M] = {};
    long long int x[M] = {};

    cout << "Initializing arrays" << endl;
    initializeArrays(A, x);
    cout << "Finished initializing arrays" << endl;

    cout << "Beginning prf" << endl;
    long long int* randomNum = prf(A, x);
    cout << "end of prf" << endl << endl;

    for(int i = 0; i < N; i++){
        cout << randomNum[i] << " ";
    }

    return;
}

void testRealPRF(){
    initializeArraysIntoFiles();
    double *randomNum = prf_from_files();

     for(int i = 0; i < N; i++){
        cout << randomNum[i] << " ";
    }

    return;
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

double* prf_from_files(){
    double x[M] = {};

    long long int temp_val;
    int i = 0;
    ifstream x_file;
    x_file.open("x.txt");
    while(x_file >> temp_val){
        x[i] = temp_val;
        i++;
    }
    x_file.close();


    static double rand[N];
    //double ratio = q / p;

    // double temp_product = 0;
    double row_vector[M] = {};

    string line;
    ifstream A_file;
    int count = 0;
    int linecount = 0;
    A_file.open("A.txt");
    while(A_file){
        getline(A_file, line);
        stringstream stream(line);
        
        long long int element;

        while(stream >> element){
            row_vector[count] = element;
            count++;
        }
        count = 0;


        // do the calculation you need for this line
        for(int i = 0; i < M; i++){
            rand[linecount] += (row_vector[i] * x[linecount]);
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
    //cout << "in PRF" << endl;
    static long long int rand[N];
    //double ratio = q/p;
    double temp = 0;

    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            temp += (A[i][j] * x[j]);
        }

        // // // rounding to p
        //     // // dumb way: x = rand[i]
        //     // // ratio while loop until we get the answer
            int count = 1;

            cout << "Start rounding" << endl;
            while(count *  ratio < temp){
                count++;
                //cout << "ratio: " << ratio << endl;
            }
            cout << "End rounding" << endl;

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