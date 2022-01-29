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

void print_key(AES_KEY &key){
  cout << "Printing key in blocks" << endl;
  for(auto item: key.rd_key){
    print((unsigned __int128) item);
    cout << endl;
  }
  cout << "Done printing key" << endl;
}

void bin(__int128 n)
{
    /* step 1 */
    if (n > 1)
        bin(n / 2);
 
    /* step 2 */
    n = n %2;
    print((unsigned __int128) n);
}

  void bin(uint64_t n)
{
    /* step 1 */
    if (n > 1)
        bin(n / 2);
 
    /* step 2 */
    cout << n % 2;
}

int main(){
	// PseudoRandomGenerator prg;
	// __int128 seed = 1000;

	// __int128 test = prg.pm_bounded(seed);
	// //cout << sizeof(test) << endl;
	// print((unsigned) test);
  //cout << sizeof(intmax_t) << endl;
  //cout << sizeof(uintmax_t) << endl;
  // __int128 test1 = 14;
  // unsigned __int128 test2 = -14;
  byte buffer[16];
  static ifstream urandom("/dev/urandom", std::ios::binary);
  urandom.read(reinterpret_cast<char *>(buffer), 16);
  urandom.close();

  AES_KEY aes_key{};

  //print_key(aes_key);
  AES_128_Key_Expansion(buffer, &aes_key);
  //print_key(aes_key);

  block blk;
  //print((unsigned __int128)blk);
  //cout << endl;
  AES_ecb_encrypt_blk(&blk, &aes_key);
  __int128 num = (__int128) blk;
  cout << "num: ";
  bin(num);
  uint64_t val = 0;
  val = val | num;
  cout << "val1: ";
  bin(val);
  //cout << "val: " << val << endl;
  val = 0;
  num = num >> 64;
  val = val | num;
  cout << "val2: ";
  bin(val);
  //cout << "val2: " << val << endl; 
  //print((unsigned __int128)blk);
}

