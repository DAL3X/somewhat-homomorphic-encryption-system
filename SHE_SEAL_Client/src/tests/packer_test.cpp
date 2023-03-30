#include "seal/seal.h"
#include "../homoenc/packer.cpp"
#include "../homoenc/seal_interface.cpp"
#include <inttypes.h>

using namespace std;
using namespace seal;

/**
 * Tests the pack and unpack routine by calculating a ciphertext, packing it, unpacking the result and performing a calculation with it.
*/
void test_packer() {
    vector<uint64_t>  a = {3, 40};
    vector<uint64_t>  b = {5, 20};
    Ciphertext *cipher_a = build_ciphertext(a);
    Ciphertext *cipher_b = build_ciphertext(b);
    
    IN_CIPHERTEXT *packed_data = pack_data(cipher_a, cipher_b);
    unpack_data(packed_data, cipher_a, cipher_b);
    sw_multiply_inplace(cipher_a, cipher_b);

    BatchEncoder encoder(*context);
    Decryptor decryptor(*context, *secret_key);
    Plaintext plain_result;
    vector<uint64_t> pod_result;
    decryptor.decrypt(*cipher_a, plain_result);
    encoder.decode(plain_result, pod_result);
    if ((pod_result[0] == a[0] * b[0]) && (pod_result[1] == a[1] * b[1]))
    {
        cout << "Packing and unpacking working as expected." << endl;
    } 
    else
    {
        cout << "WARNING: Packing and unpacking not working as expected!" << endl;
    }
    delete cipher_a;
}