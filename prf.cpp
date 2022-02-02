#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "aes.hpp"
#include "prg.hpp"



#define M 1000 // change to 500 for uint64_t version
#define N 500 // change to 250 for uint64_t version
#define p 65535 //(2^16-1)
#define q 4294967296 // (2^32)...
#define ratio q/p



using namespace std;
using namespace LWERandomness;

/*
    INTEGER VERSION OF THE PRF
*/
uint64_t* prf(uint64_t A[N][M], uint64_t x[M]); // main PRF function
//void initializeArrays(uint64_t A[N][M], uint64_t x[M]); // initializes random arrays
uint64_t* testIntegerPRF(); // tester
void initalizeArrayA(uint64_t A[N][M]);
void initalizeArrayX(uint64_t x[M]);


int main(int argc, char *argv[]){
   ofstream myf;
	myf.open("data.txt");
	uint64_t *result;
	uint64_t min = 65536;
	uint64_t max = 0;
	for(int i = 0; i < 1000; i++){
		result = testIntegerPRF();
		for(int j = 0; j < N; j++){
			if(result[j] > max){
				max = result[j];
			}
			if(result[j] < min){
				min = result[j];
			}
			myf << result[j] << "\n";
		}
	}

	myf.close();
	cout << "Min: " << min << endl;
	cout << "Max: " << max << endl;
    return 0;
}

uint64_t* testIntegerPRF(){
    //uint64_t A[N][M] = {};
    auto A = new uint64_t[N][M];
    uint64_t *x = new uint64_t[M];

    cout << "Initializing arrays" << endl;
    //initializeArrays(A, x);
    initalizeArrayA(A);
    initalizeArrayX(x);
    cout << "Finished initializing arrays" << endl;

    cout << "Beginning prf" << endl;
    uint64_t* randomNum = prf(A, x);
    cout << "end of prf" << endl << endl;

    // for(int i = 0; i < N; i++){
    //     cout << randomNum[i] << " ";
    // }

    return randomNum;
}

void initalizeArrayA(uint64_t A[N][M]){
	byte buffer[16];
  	static ifstream urandom("/dev/urandom", std::ios::binary);
  	urandom.read(reinterpret_cast<char *>(buffer), 16);
  	//urandom.close();

  	AES_KEY aes_key{};
  	AES_128_Key_Expansion(buffer, &aes_key);
  	block blk;
  	

  	for(int i = 0; i < N; ++i){
  		for(int j = 0; j < M; j += 4){
  			block tmp = blk;
  			AES_ecb_encrypt_blk(&blk, &aes_key);

  			__int128 num = (__int128) blk;
  			uint64_t half = 0;
  			half = half | num;
  			uint64_t quarter = half >> 32; // now we got the upper 32-bits
  			A[i][j] = quarter;
  			quarter = half << 32;
  			quarter = quarter >> 32;
  			A[i][j+1] = quarter;

  			half = 0;
  			num = num >> 64;
  			half = half | num;
  			quarter = half >> 32;
  			A[i][j+2] = quarter;
  			quarter = half << 32;
  			quarter = quarter >> 32;
  			A[i][j+3] = quarter;

  			blk = ++tmp;
  		}
  		urandom.read(reinterpret_cast<char *>(buffer), 16);
  		AES_KEY aes_key{};
  		AES_128_Key_Expansion(buffer, &aes_key);
  	}

  	urandom.close();
}

void initalizeArrayX(uint64_t x[M]){
	byte buffer[16];
  	static ifstream urandom("/dev/urandom", std::ios::binary);
  	urandom.read(reinterpret_cast<char *>(buffer), 16);
  	//urandom.close();

  	AES_KEY aes_key{};
  	AES_128_Key_Expansion(buffer, &aes_key);
  	block blk;
  	

  	for(int i = 0; i < M; i+=4){
  		block tmp = blk;
  		AES_ecb_encrypt_blk(&blk, &aes_key);

  		__int128 num = (__int128) blk;
  		uint64_t half = 0;
  		half = half | num;
  		uint64_t quarter = half >> 32; // now we got the upper 32-bits
  		x[i] = quarter;
  		quarter = half << 32;
  		quarter = quarter >> 32;
  		x[i+1] = quarter;

  		half = 0;
  		num = num >> 64;
  		half = half | num;
  		quarter = half >> 32;
  		x[i+2] = quarter;
  		quarter = half << 32;
  		quarter = quarter >> 32;
  		x[i+3] = quarter;

  		blk = tmp + 1;		

  		urandom.read(reinterpret_cast<char *>(buffer), 16);
  		AES_KEY aes_key{};
  		AES_128_Key_Expansion(buffer, &aes_key);
  	}

  	urandom.close();
 }


// void initializeArrays(uint64_t A[N][M], uint64_t x[M]){
//     // random_device rd;
//     // std::mt19937 gen(rd());
//     // uniform_int_distribution<uint64_t> distr(0, q);

//     cout << "in Initialize Arrays" << endl;
//     for(int i = 0; i < N; ++i){
//         for(int j = 0; j < M; ++j){
//             A[i][j] = distr(gen);
//             if(A[i][j] > q){
//                 cout << "Assigned an invalid element in A" << endl;
//             }
//         }
//     }

//     for(int i = 0; i < M; i++){
//         x[i] = distr(gen);
//         if(x[i] > q){
//                 cout << "Assigned an invalid element in A" << endl;
//         }
//     }
// }


uint64_t* prf(uint64_t A[N][M], uint64_t x[M]){
    //cout << "in PRF" << endl;
    uint64_t *rand = new uint64_t[N];
    uint64_t temp = 0;

    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            temp = (temp + ((A[i][j] * x[j]))) % q;
        }

        // // // rounding to p
        //     // // dumb way: x = rand[i]
        //     // // ratio while loop until we get the answer
        	uint64_t increment = (uint64_t) ratio;

        	rand[i] = temp / increment;
        	
            // uint64_t count = 1;
            // uint64_t multiple = (uint64_t) ratio;

            // //cout << "Start rounding" << endl;
            // // while(count *  multiple < temp && temp != 0){
            // //     count++;
            // // }
            // // //cout << "End rounding" << endl;

            // // if(count - 1 == 0){
            // //     //cout << "ratio: " << ratio << "temp: " << temp << endl; 
            // //     rand[i] = count;
            // // }
            // // else{
            // //     rand[i] = count-1;
            // // }
           
           
            // count = 1;
            // temp = 0;
        // }
    }

    return rand;
}