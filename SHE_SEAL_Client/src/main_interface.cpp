#include "homoenc/seal_interface.cpp"
#include "homoenc/packer.cpp"
#include "util/parameters.hpp"
#include "network/socket_client.cpp"
#include <iostream>

#include "constants.h"

using namespace std;

    int batch_size = 1; // Number of slots in plaintext.
    vector<uint64_t> input1; // Inputs for local ciphertext 1.
    vector<uint64_t> input2; // Inputs for local ciphertext 2.
    int operation = 1; // (0:Addition) (1:Multiplication)
    bool hold = false; // Decides if the results are used for next calculation.
    Ciphertext *cipher1; // local ciphertext 1 for operations.
    Ciphertext *cipher2; // local ciphertext 2 for operations.


/**
 * Enables the user to input numbers to form local ciphertext 1 and local ciphertext 2 
 * and input the operation which should be performed.
*/
void gather_inputs()
{
    input1.clear();
    input2.clear();
    cout << "Enter batch size: ";
    cin >> batch_size;
    cout << "Enter the first data vector: ";
    int number;
    for(int i = 0; i < batch_size; i++)
    { 
        cin >> number;
        input1.push_back(number);
    }
    cout << "Enter the second data vector: ";
    for(int i = 0; i < batch_size; i++)
    { 
        cin >> number;
        input2.push_back(number);
    }
    cout << "Enter the operation (0:add) (1:multiply): ";
    cin >> operation;
}

/**
 * Enables the user to input numbers to form local ciphertext 2 and input the operation which should be performed.
 * Local ciphertext 1 is used from the last calculation.
*/
void gather_input()
{
    input2.clear();
    int number;
    cout << "Enter the second data vector: ";
    for(int i = 0; i < batch_size; i++)
    { 
        cin >> number;
        input2.push_back(number);
    }
    cout << "Enter the operation (0:add) (1:multiply): ";
    cin >> operation;
}

/**
 * Prompts the user to set if the result should be used as local ciphertext 1 in the next calculation.
*/
void set_hold()
{
    char cont;
    cout << "Do you want to continue calculating with the result? (y/n): ";
    cin >> cont;
    if (cont == 'y') 
    {
        hold = true;
    }
    else
    {
        hold = false;
    }
}

/**
 * Prompts the user to set if he wants to quit the program.
 * The returned value is the user decision.
*/
bool ask_quit()
{
    char cont;
    cout << "Do you want to quit? (y/n): ";
    cin >> cont;
    if (cont == 'y') 
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Runs the interface the user interacts with.
*/
void runInterface() 
{
	cout << "Client ready!" << endl;
    while (true)
    {
        if (hold)
        {
            gather_input();
            delete cipher2;
            cipher2 = build_ciphertext(input2);
        } 
        else
        {
            gather_inputs();
            delete cipher1;
            delete cipher2;
            cipher1 = build_ciphertext(input1);
            cipher2 = build_ciphertext(input2);
        }
        if (operation == 0)
        {
            sw_add_inplace(cipher1, cipher2);
        }
        else
        {
			OUT_CIPHERTEXT *out_cipher = new OUT_CIPHERTEXT();
			IN_CIPHERTEXT *in_cipher = pack_data(cipher1, cipher2);
			send_ciphertext(in_cipher);
			cout << "Calculating on server side. Please wait..." << endl;
			rec_ciphertext(out_cipher);
			delete(in_cipher);
			unpack_data(out_cipher, cipher1);
            //sw_multiply_inplace(cipher1, cipher2);
        }
        vector<uint64_t> plaintext = get_plaintext(cipher1, batch_size);
        cout << "Results: ";
        for (int i = 0; i < batch_size; i++)
        {
            cout << plaintext[i] << " ";
        }
        cout << endl;
        set_hold();
        if (!hold && ask_quit())
        {
            break;
        }
    }
    delete cipher1;
    if (!hold)
    {
        delete cipher2;
    }
}

/**
 * Sends the calculated relinearisation key to the server.
 * SEAL has to be initialised and the client must be connected to the server.
*/
void prepare_relin()
{
	cout << "Performing preliminary computations..." << endl;
	RLK_CONSTANTS *rlk = pack_relin(relin_key);
	send_relin(&rlkconstants);
	delete rlk;
}


/**
 * Main routine sets everything up and runs the program.
*/
int main() {
	cout << "=== SHE Client ===" << endl;
    init_SEAL();
    open_connection();
	send_relin(&rlkconstants);
	//prepare_relin();
	runInterface();
	close_connection();
    close_SEAL();
    return 0;
}
