#ifndef SEAL_INTERFACE
#define SEAL_INTERFACE

#include "seal/seal.h"
#include "../util/parameters.hpp"
#include "hardware/data.hpp"
#include <iostream>
#include <vector>
#include <inttypes.h>

using namespace std;
using namespace seal;

SEALContext *context;  // The SEAL context used for most operations.
PublicKey *public_key; // The public key to encrypt data.
SecretKey *secret_key; // The secret key to decrypt data.
RelinKeys *relin_key;  // The relinearisation key to use relinearisation.

SEALContext *switching_context; // The SEAL context used only for modulo switching.

/**
 * Initializes the components needed for modulo switching down ciphertexts.
*/
int init_modulo_switching()
{
	EncryptionParameters parameters(scheme_type::bfv);
    size_t poly_modulus_degree = POLY_DEG;
    parameters.set_poly_modulus_degree(poly_modulus_degree);
    vector<Modulus> prime_mods;
    for (int i = 0; i < NUMPRIMES; i++)
    {
        prime_mods.push_back(Modulus(primes[i]));
    }
    prime_mods.push_back(Modulus(special_prime));
	prime_mods.push_back(Modulus(switching_prime));
    parameters.set_coeff_modulus(prime_mods);
    parameters.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, PLAIN_PRIME));
    switching_context = new SEALContext(parameters, true, sec_level_type::none);
	return 0;
}

/**
 * Initializes all components needed for Microsoft SEAL.
*/
int init_SEAL()
{
    EncryptionParameters parameters(scheme_type::bfv);
    size_t poly_modulus_degree = POLY_DEG;
    parameters.set_poly_modulus_degree(poly_modulus_degree);
    vector<Modulus> prime_mods;
    for (int i = 0; i < NUMPRIMES; i++)
    {
        prime_mods.push_back(Modulus(primes[i]));
    }
    prime_mods.push_back(Modulus(special_prime));
    parameters.set_coeff_modulus(prime_mods);
    // Enable batching
    parameters.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, PLAIN_PRIME));
    // Disabling security check since only 80-bit security is possible with this parameters
    context = new SEALContext(parameters, false, sec_level_type::none);

    // Using copy-constructors to prevent uninitialized memory pools
    KeyGenerator keygen(*context);
    SecretKey sk = keygen.secret_key();
    secret_key = new SecretKey(sk);
    PublicKey pk;
    keygen.create_public_key(pk);
    public_key = new PublicKey(pk);
    RelinKeys relin;
    keygen.create_relin_keys(relin);
    relin_key = new RelinKeys(relin);

	init_modulo_switching();
    cout << "Succefully initialized SEAL" << endl;
    return 0;
}

/**
 * Deletes all components needed for Microsoft SEAL.
*/
int close_SEAL() 
{
    delete context;
    delete public_key;
    delete secret_key;
    delete relin_key;
    return 0;
}

/**
 * Generates and reserves memory for a ciphertext. 
 * If values is longer than the polynomial degree d, only the first d elements will be included.
*/
Ciphertext *build_ciphertext(vector<uint64_t> values)
{
    if (values.size() > POLY_DEG) 
    {
        cout << "Too many values! Limit is set to: " << POLY_DEG << " coefficients" << endl;
    }
    BatchEncoder encoder(*context);
    Encryptor encryptor(*context, *public_key);
    size_t slot_count = encoder.slot_count();
    size_t row_size = slot_count / 2;
    vector<uint64_t> pod_matrix(slot_count, 0ULL);
    for (int i = 0; (i < values.size()) && (i < POLY_DEG); i++) 
    {
        pod_matrix[i] = values[i];
    }
    Plaintext plain_matrix;
    encoder.encode(pod_matrix, plain_matrix);
    Ciphertext *encrypted_matrix = new Ciphertext();
    encryptor.encrypt(plain_matrix, *encrypted_matrix);
    return encrypted_matrix;
}

/**
 * Returns the plaintext with given length corresponding to a ciphertext.
*/
vector<uint64_t> get_plaintext(Ciphertext *ciphertext, int length)
{
    BatchEncoder encoder(*context);
    Decryptor decryptor(*context, *secret_key);
    Plaintext plain_result;
    vector<uint64_t> pod_result;
    decryptor.decrypt(*ciphertext, plain_result);
    encoder.decode(plain_result, pod_result);
    vector<uint64_t> ret(pod_result.begin(), pod_result.begin() + length);
    return ret;
}

/**
 * Transforms a ciphertext that is part of a relinearisation key from NTT into coefficient format and goes down the modulo chain once (RNS = RNS-1).
 * After that the ciphertext that is part of a relinearisation key gets transformed back into NTT format.
*/
void transform_relin_cipher(Ciphertext *ciphertext)
{
	Evaluator evaluator(*switching_context);
	evaluator.transform_from_ntt_inplace(*ciphertext);
	evaluator.mod_switch_to_inplace(*ciphertext, switching_context->key_context_data()->next_context_data()->next_context_data().get()->parms_id());
	evaluator.transform_to_ntt_inplace(*ciphertext);
}

/**
 * Multiplies and relinearises two numbers in software. 
 * The result is put into a.
*/
void sw_multiply_inplace(Ciphertext *a, Ciphertext *b)
{
    Evaluator evaluator(*context);
    evaluator.multiply_inplace(*a, *b);
    evaluator.relinearize_inplace(*a, *relin_key);
}

/**
 * Adds two numbers in software. 
 * The result is put into a.
*/
void sw_add_inplace(Ciphertext *a, Ciphertext *b)
{
    Evaluator evaluator(*context);
    evaluator.add_inplace(*a, *b);
}

#endif
