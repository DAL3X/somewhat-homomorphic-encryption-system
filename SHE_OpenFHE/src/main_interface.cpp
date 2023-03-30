#include "util/parameters.hpp"
#include <iostream>
#include "openfhe.h"
#include <chrono>

using namespace lbcrypto;
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

/**
 * Main routine sets everything up and runs the program.
*/
int main() 
{
    CCParams<CryptoContextBFVRNS> parameters;
    parameters.SetPlaintextModulus(PLAIN_PRIME);

    /* The multiplicative depth has to be set accordingly so OpenFHE actually generates the required number of primes
    *  Unfortunately there is no way of setting the required coprimes yourself.
    *  On one hand this makes OpenFHE easier to use, since the user does not have to understand or calculate the coprime selection themselves.
    *  On the other hand this holds OpenFHE back from being used in any scenarios, where we might want to set the specific primes by hand.
    */
    parameters.SetMultiplicativeDepth(3); // Set this to get 6 coprimes for the RNS.

    parameters.SetRingDim(POLY_DEG);
    parameters.SetFirstModSize(30);   // Sets the bitsize of the first RNS coprime
    parameters.SetScalingModSize(30); // Sets the bitsize of the other RNS coprime. This is not in one value because of the way OpenFHE implements the HPS scheme.
    parameters.SetSecurityLevel(HEStd_NotSet); // Disable security level, since our parameters are not conform with the Homomorphic Encryption Standard.

    CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);
    cryptoContext->Enable(ADVANCEDSHE);


    int rns_size = cryptoContext->GetCryptoParameters()->GetElementParams()->GetParams().size(); // Accesses the number of coprimes for the RNS
    if (rns_size == NUMPRIMES)
    {
        cout << "Succefully initiated a RNS with " << NUMPRIMES << " coprimes." << endl;
    }
    else
    {
        cout << "Initiation of the RNS with " << NUMPRIMES << " coprimes failed!" << endl;
        return 1;
    } 

    cout << "The used coprimes are: ";
    for (int i = 0; i < rns_size; i++)
    {
        cout << cryptoContext->GetCryptoParameters()->GetElementParams()->GetParams()[i].get()->GetModulus() << " "; // Accesses the coprimes for the RNS directly.
    } 
    cout << endl;

    // Now generate a Ciphertext to see if the RNS representation actually fits our parameters.
    KeyPair<DCRTPoly> keyPair;
    keyPair = cryptoContext->KeyGen();
	cryptoContext->EvalMultKeysGen(keyPair.secretKey);

    std::vector<int64_t> vectorOfInts1 = {5};
	std::vector<int64_t> vectorOfInts2 = {42};

	auto t1 = high_resolution_clock::now();
    Plaintext plaintext1 = cryptoContext->MakePackedPlaintext(vectorOfInts1); // Batching to build the plaintext.
	Ciphertext<DCRTPoly> cipher1 = cryptoContext->Encrypt(keyPair.publicKey, plaintext1); // Encrypting the plaintext to get the ciphertext. It contains two DCRTPoly(nomials) ,which means RNS format.
 	auto t2 = high_resolution_clock::now();
	duration<double, std::milli> ms_double = t2 - t1;
 	cout << "Time needed for the batching and encryption: " << ms_double.count() << "ms" << endl;

    Plaintext plaintext2 = cryptoContext->MakePackedPlaintext(vectorOfInts2); 
	Ciphertext<DCRTPoly> cipher2 = cryptoContext->Encrypt(keyPair.publicKey, plaintext2); 

	t1 = high_resolution_clock::now();
	auto ciphertextAdd12 = cryptoContext->EvalAdd(cipher1, cipher2);
 	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
 	cout << "Time needed for the addition: " << ms_double.count() << "ms" << endl;

	t1 = high_resolution_clock::now();
	auto ciphertextMul12 = cryptoContext->EvalMult(cipher1, cipher2); // Multiply and relinearise in one
 	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
 	cout << "Time needed for the multiplication and relinearisation: " << ms_double.count() << "ms" << endl;
	
    Plaintext plaintextAddResult;
 	Plaintext plaintextMultResult;

	t1 = high_resolution_clock::now();
	cryptoContext->Decrypt(keyPair.secretKey, ciphertextMul12, &plaintextMultResult);
 	t2 = high_resolution_clock::now();
	ms_double = t2 - t1;
 	cout << "Time needed for the decryption and decoding: " << ms_double.count() << "ms" << endl;

    cryptoContext->Decrypt(keyPair.secretKey, ciphertextAdd12, &plaintextAddResult);

    return 0;
}
