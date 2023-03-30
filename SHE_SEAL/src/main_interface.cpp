#include "homoenc/seal_interface.cpp"
#include "homoenc/packer.cpp"
#include "util/parameters.hpp"
#include <iostream>
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

/**
 * Main routine sets everything up and runs the timings/tests.
*/
int main() {
	cout << "=== SEAL Demo ===" << endl;
    init_SEAL();

	vector<uint64_t>  a = {5};
    vector<uint64_t>  b = {42};
    Ciphertext *cipher_a = build_ciphertext(a);
    Ciphertext *cipher_b = build_ciphertext(b);  
	Ciphertext *cipher_c = build_ciphertext(a);  
	
	auto t1 = high_resolution_clock::now();
	IN_CIPHERTEXT *icp = pack_data(cipher_a, cipher_b); 
 	auto t2 = high_resolution_clock::now();
	duration<double, std::milli> ms_double = t2 - t1;
 	cout << "Time needed for packing: " << ms_double.count() << "ms" << endl;

	sw_multiply_inplace(cipher_a, cipher_b);

	vector<uint64_t> plaintext = get_plaintext(cipher_a, 1);
	int result = plaintext[0];
    cout << "5x42 result: " << result << endl;

	sw_add_inplace(cipher_c, cipher_b);
	vector<uint64_t> add_plaintext = get_plaintext(cipher_c, 1);
	int add_result = add_plaintext[0];
    cout << "5+42 result: " << add_result << endl;	
	

	t1 = high_resolution_clock::now();
	unpack_data_in_ct(icp, cipher_a);
 	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
 	cout << "Time needed for unpacking: " << ms_double.count() << "ms" << endl;


	sw_multiply_inplace(cipher_a, cipher_b);

	plaintext = get_plaintext(cipher_a, 1);
	if(plaintext[0] == result)
	{
		cout << "Un/Packing returned the expected result" << endl;
	}
	else
	{
		cout << "Un/Packing did NOT return the expected result!!!" << endl;
		cout << "Returned: " << plaintext[0] << endl;
	}
	
    close_SEAL();
    return 0;
}
