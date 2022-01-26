#include "aes.hpp"
#include "prg.hpp"
#include <iostream>

using namespace LWERandomness;
using namespace std;

void print(unsigned __int128 x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x > 9) print(x / 10);
    putchar(x % 10 + '0');
}

int main(){
	PseudoRandomGenerator prg;
	__int128 seed = 1000;

	__int128 test = prg.pm_bounded(seed);
	//cout << sizeof(test) << endl;
	print((unsigned) test);
  //cout << sizeof(intmax_t) << endl;
  //cout << sizeof(uintmax_t) << endl;
  // __int128 test1 = 14;
  // unsigned __int128 test2 = -14;
}

