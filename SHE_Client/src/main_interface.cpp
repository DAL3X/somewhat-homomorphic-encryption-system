#include "util/parameters.hpp"
#include "network/socket_client.cpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

#include "constants.h"
#include "data0.h"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


/**
 * Main routine sets everything up and runs the program.
*/
int main() {
	cout << "=== SHE Client ===" << endl;
    open_connection();

	auto t1 = high_resolution_clock::now();
    send_relin(&rlkconstants);
 	auto t2 = high_resolution_clock::now();
	duration<double, std::milli> ms_double = t2 - t1;
 	cout << "Time needed for sending the relinearisation key: " << ms_double.count() << "ms" << endl;

	cout << "Relinearisation key was send" << endl;
	sleep(1);
    IN_CIPHERTEXT in = in_ct0[0];

	t1 = high_resolution_clock::now();
    send_ciphertext(&in);
 	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
 	cout << "Time needed for sending the ciphertext: " << ms_double.count() << "ms" << endl;

	cout << "Ciphertext was send" << endl;
	sleep(1);
    OUT_CIPHERTEXT result;

    rec_ciphertext(&result);
	cout << "Ciphertext was revieved" << endl;

	int suc = 1;
    for (int i = 0; i < NUMPRIMES; i++)
	for (int j = 0; j < PACKED_POLY_DEG; j++)
	{
		if (result.c0[i][j] != out_ctf.c0[i][j] || result.c1[i][j] != out_ctf.c1[i][j])
		{
			suc = 0;
		}
	}
	if (suc == 1)
	{
		cout << "Passed the calculation test: All results are correct!" << endl;
	}
	close_connection();
    return 0;
}
