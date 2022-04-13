#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "aes.hpp"
#include "prg.hpp"
#include "prf.cpp"

#define sqrt_N_dpf 32

using namespace std;
using namespace LWERandomness;


class Share {
	public:
		bitset<sqrt_N_dpf> bits; //need to change if there are more bits
		uint64_t seeds[sqrt_N_dpf];
		Share(const Share& source): Share{ *source.bits, *source.seeds}{}

		~Share(){
			delete bits;
			delete seeds;
		}
}