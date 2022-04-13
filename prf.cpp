#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "aes.hpp"
#include "prg.hpp"
#include "share.cpp"



#define M 1000 // change to 500 for uint64_t version
#define N 500 // change to 250 for uint64_t version
#define p 65535 //(2^16-1)
#define q 4294967296 // (2^32)...
#define ratio q/p
#define N_dpf 1024
#define sqrt_N_dpf 32
#define number_of_servers 2


using namespace std;
using namespace LWERandomness;

/*
    INTEGER VERSION OF THE PRF
*/
uint64_t* prf(uint64_t A[N][M], uint64_t x[M]); // main PRF function
uint64_t* testIntegerPRF(); // tester
void initalizeArrayA(uint64_t A[N][M]); // initializes matrix A
void initalizeArrayX(uint64_t x[M]); 	// initializes vector x which serves as the key for the PRG (later on)

uint64_t prg(uint64_t seed[M]);
auto A_matrix = new uint64_t[N][M];
int counter = 0;
uint64_t* create_ej(uint64_t j);
Share* dpf(uint64_t x);


static ifstream urandom("/dev/urandom", std::ios::binary);

int main(int argc, char *argv[]){
 //   ofstream myf;
	// myf.open("data.txt");
	// uint64_t *result;
	// uint64_t min = 65536;
	// uint64_t max = 0;
	// for(int i = 0; i < 1000; i++){
	// 	result = testIntegerPRF();
	// 	for(int j = 0; j < N; j++){
	// 		if(result[j] > max){
	// 			max = result[j];
	// 		}
	// 		if(result[j] < min){
	// 			min = result[j];
	// 		}
	// 		myf << result[j] << "\n";
	// 	}
	// }

	// myf.close();
	// cout << "Min: " << min << endl;
	// cout << "Max: " << max << endl;
 //    return 0;
	// ofstream myf;
	// myf.open("prgdata.txt");
	// auto x = new uint64_t[M];

	// for(int i = 0; i < 1000000; i++){
	// 	if(i % 100000 == 0)
	// 		cout << "on " << i << endl;
	// 	initalizeArrayX(x);
	// 	uint64_t val = prg(x);
	// 	myf << val << "\n";
	// 	//cout << "val: " << val << endl << endl;
	// }

	// urandom.close();
	// myf.close();
	// initializeDomainDPF(domain);
	// for(int i = 0; i < sqrt_N_dpf; i++){
	// 	for(int j = 0; j < sqrt_N_dpf; j++){
	// 		cout << domain[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	return 0;
}

Share* dpf(uint64_t x){
  uint64_t i_star = x / sqrt_N_dpf;
  uint64_t j = x % sqrt_N_dpf;
  auto ej = new uint64_t[sqrt_N_dpf];
  ej[j] = 1;
  
  auto shares = new Share[number_of_servers];
  for(int i = 0; i < number_of_servers-1; i++){
    Share s = new Share();
    // need to randomize bits except for bit i
    // need to reandomize seeds except for seed i?

    for(int j = 0; j < sqrt_N_dpf; j+=2){
      uint64_t* seeds = randomSeed();
      s.seeds[j] = seeds[i];
      s.seeds[j+1] = seeds[j+1];
    }

    shares[i] = s;
  }

  uint64_t t = 0;
  Share final = new Share();
  uint32_t* bits = new uint32_t[2];
  // now time to deal with the last share
  for(int i = 0; i < sqrt_N_dpf; i+=2){
  	if(i != i_star){
  		uint64_t seed_i = 0;
  		for(int server = 0; j < number_of_servers-1; server++){
  			seed_i ^= server.seeds[i];
  		}

  		final.seeds[i] = seed_i;
  	}
  	else{
  		final.seeds[i] = randomSeed()[0];
  	}
  	
  }
}



uint64_t* randomSeed(){
  auto seeds = new uint64_t[2];
  byte buffer[16];
  urandom.read(reinterpret_cast<char *>(buffer), 16);
  AES_KEY aes_key{};
  AES_128_Key_Expansion(buffer, &aes_key);
  block blk;

  block tmp = blk;
  AES_ecb_encrypt_blk(&blk, &aes_key);

  __int128 num = (__int128) blk;
  uint64_t half = 0;
  half = half | num;

  seeds[0] = half;
  half = 0;
  num = num >> 64;
  half = half | num;
  seeds[1] = half;
  return seeds;
}



uint64_t* testIntegerPRF(){
    auto A = new uint64_t[N][M];
    uint64_t *x = new uint64_t[M];

    cout << "Initializing arrays" << endl;
    initalizeArrayA(A);
    initalizeArrayX(x);
    cout << "Finished initializing arrays" << endl;

    cout << "Beginning prf" << endl;
    uint64_t* randomNum = prf(A, x);
    cout << "end of prf" << endl << endl;

    return randomNum;
}

void initalizeArrayA(uint64_t A[N][M]){
	byte buffer[16];
  	//static ifstream urandom("/dev/urandom", std::ios::binary);
  	urandom.read(reinterpret_cast<char *>(buffer), 16);

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

  	//urandom.close();
}

void initalizeArrayX(uint64_t x[M]){
	byte buffer[16];
  	
  	urandom.read(reinterpret_cast<char *>(buffer), 16);

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

  	//urandom.close();
 }


uint64_t* prf(uint64_t A[N][M], uint64_t x[M]){
    uint64_t *rand = new uint64_t[N];
    uint64_t temp = 0;

    for(int i = 0; i < N; ++i){
        for(int j = 0; j < M; ++j){
            temp = (temp + ((A[i][j] * x[j]))) % q;
        }

        // rounding to p
        uint64_t increment = (uint64_t) ratio;

        rand[i] = temp / increment;
    }

    return rand;
}

uint64_t prg(uint64_t seed[M]){
	if(counter == 0){
		initalizeArrayA(A_matrix);
		counter++;
	}

	byte buffer[4];
	unsigned int point;
  	//static ifstream urandom("/dev/urandom", std::ios::binary);
  	urandom.read(reinterpret_cast<char *>(buffer), 4);
  	//urandom.close();

  	memcpy(&point, buffer, sizeof(int)); // got random point
  	//cout << "point (before rounding): " << point << endl;
  	point = point % N;
  	//cout << "point (after rounding): " << point << endl;


  	uint64_t* result = prf(A_matrix, seed);

  	return result[point];
}