#ifndef PACKER
#define PACKER

#include "seal/seal.h"
#include "../util/parameters.hpp"
#include "seal_interface.cpp"
#include "hardware/data.hpp"
#include <inttypes.h>

using namespace std;
using namespace seal;

/**
 * Packs and reserves memory for a packed ciphertext.
 * A packed ciphertext represents two ciphertexts in the hardware input format.
 * The coefficients must be packed as described in https://eprint.iacr.org/archive/2013/866/1390397338.pdf.
*/
IN_CIPHERTEXT *pack_data (Ciphertext *cipher1, Ciphertext *cipher2)
{
    IN_CIPHERTEXT *in_cipher = new IN_CIPHERTEXT();
    // 2 polynomials with NUMPRIMES RNS slices and POLY_DEG coefficients
    if (cipher1->dyn_array().size() != 2*NUMPRIMES*POLY_DEG || cipher2->dyn_array().size() != 2*NUMPRIMES*POLY_DEG)
    {
        cout << "Wrong parameters set. Packing not possible!" << endl;
        return in_cipher;
    }
    
    for (int cipher_iter = 0; cipher_iter < 2; cipher_iter++)
    {
        for (int poly_iter = 0; poly_iter < 2; poly_iter++)
        {
            for (int rns_iter = 0; rns_iter < NUMPRIMES; rns_iter++)
            {
                for (int coeff_iter = 0; coeff_iter < POLY_DEG; coeff_iter = coeff_iter+2) 
                {
                    int index = rns_iter * POLY_DEG + coeff_iter;
                    int packed_index = coeff_iter/2;
                    if (cipher_iter == 0)
                    {
                        uint64_t data_low = cipher1->data(poly_iter)[index];
                        uint64_t data_high = cipher1->data(poly_iter)[index+1];
                        // Now put the numbers into the corresponding places by placing two 30 bit numbers into one 60 bit slot
                        if (poly_iter == 0)
                        {
                            in_cipher->c00[rns_iter][packed_index] = data_low | (data_high << 30);
                        }
                        else
                        {
                            in_cipher->c01[rns_iter][packed_index] = data_low | (data_high << 30);
                        }
                    }
                    else
                    {
                        uint64_t data_low = cipher2->data(poly_iter)[index];
                        uint64_t data_high = cipher2->data(poly_iter)[index+1];
                        // Now put the numbers into the corresponding places by placing two 30 bit numbers into one 60 bit slot
                        if (poly_iter == 0)
                        {
                            in_cipher->c10[rns_iter][packed_index] = data_low | (data_high << 30);
                        }
                        else
                        {
                            in_cipher->c11[rns_iter][packed_index] = data_low | (data_high << 30);
                        }
                    }
                }
            }
        }
    }
    return in_cipher;
}

/**
 * Unpacks and releases memory for a packed ciphertext.
 * The packed ciphertext represents one ciphertext in the hardware input format.
 * The coefficients must be unpacked as described in https://eprint.iacr.org/archive/2013/866/1390397338.pdf.
*/
void unpack_data (OUT_CIPHERTEXT *packed, Ciphertext *cipher1)
{
    // NUMPRIMES RNS slices and POLY_DEG coefficients
    if (cipher1->dyn_array().size() != 2*NUMPRIMES*POLY_DEG)
    {
        cout << "Wrong parameters set. Unpacking not possible!" << endl;
        return;
    }
    
    for (int poly_iter = 0; poly_iter < 2; poly_iter++)
    {
        for (int rns_iter = 0; rns_iter < NUMPRIMES; rns_iter++)
        {
            for (int coeff_iter = 0; coeff_iter < POLY_DEG; coeff_iter = coeff_iter+2) 
            {
                int index = rns_iter * POLY_DEG + coeff_iter;
                int packed_index = coeff_iter/2;
                // Now put the numbers into the corresponding places by placing one 60 bit numbers into two 30 bit slot
                if (poly_iter == 0)
                {
                    uint64_t data_packed = packed->c0[rns_iter][packed_index];
                    cipher1->data(poly_iter)[index] = (data_packed & 0x3FFFFFFF);
                    cipher1->data(poly_iter)[index+1] = (data_packed >> 30);
                }
                else
                {
                    uint64_t data_packed = packed->c1[rns_iter][packed_index];
                    cipher1->data(poly_iter)[index] = (data_packed & 0x3FFFFFFF);
                    cipher1->data(poly_iter)[index+1] = (data_packed >> 30);
                }
            }
        }
    }
    delete packed;
}

/**
 * The same as unpack_data, but the packed data is a the first ciphertext in an IN_CIPHERTEXT for testing.
*/
void unpack_data_in_ct (IN_CIPHERTEXT *packed, Ciphertext *cipher1)
{
    // NUMPRIMES RNS slices and POLY_DEG coefficients
    if (cipher1->dyn_array().size() != 2*NUMPRIMES*POLY_DEG)
    {
        cout << "Wrong parameters set. Unpacking not possible!" << endl;
        return;
    }
    
    for (int poly_iter = 0; poly_iter < 2; poly_iter++)
    {
        for (int rns_iter = 0; rns_iter < NUMPRIMES; rns_iter++)
        {
            for (int coeff_iter = 0; coeff_iter < POLY_DEG; coeff_iter = coeff_iter+2) 
            {
                int index = rns_iter * POLY_DEG + coeff_iter;
                int packed_index = coeff_iter/2;
                // Now put the numbers into the corresponding places by placing one 60 bit numbers into two 30 bit slot
                if (poly_iter == 0)
                {
                    uint64_t data_packed = packed->c00[rns_iter][packed_index];
                    cipher1->data(poly_iter)[index] = (data_packed & 0x3FFFFFFF);
                    cipher1->data(poly_iter)[index+1] = (data_packed >> 30);
                }
                else
                {
                    uint64_t data_packed = packed->c01[rns_iter][packed_index];
                    cipher1->data(poly_iter)[index] = (data_packed & 0x3FFFFFFF);
                    cipher1->data(poly_iter)[index+1] = (data_packed >> 30);
                }
            }
        }
    }
    delete packed;
}


/**
 * Packs and reserves memory for packed relinearisation keys.
 * It also changes the RNS from seven to six coprimes.
 * Packed relinearisation keys represent the six relin-ciphertexts in the hardware input format.
 * The coefficients must be packed as described in https://eprint.iacr.org/archive/2013/866/1390397338.pdf.
*/
RLK_CONSTANTS *pack_relin (RelinKeys *relin)
{
    RLK_CONSTANTS *relin_constants = new RLK_CONSTANTS();
    for (int cipher_iter = 0; cipher_iter < 6; cipher_iter++)
    {
		Ciphertext cipher = relin->key(2)[cipher_iter].data();

        for (int poly_iter = 0; poly_iter < 2; poly_iter++)
        {
            for (int rns_iter = 0; rns_iter < NUMPRIMES; rns_iter++)
            {
                for (int coeff_iter = 0; coeff_iter < POLY_DEG; coeff_iter = coeff_iter+2) 
                {
                    int index = rns_iter * POLY_DEG + coeff_iter;
                    int packed_index = coeff_iter/2;
                    uint64_t data_low = cipher.data(poly_iter)[index];
                    uint64_t data_high = cipher.data(poly_iter)[index+1];
                    
                    switch(cipher_iter) 
                    {
                        case 0:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk00[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk01[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        case 1:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk10[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk11[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        case 2:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk20[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk21[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        case 3:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk30[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk31[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        case 4:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk40[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk41[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        case 5:
                            if (poly_iter == 0)
                            {
                                relin_constants->rlk50[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            else
                            {
                                relin_constants->rlk51[rns_iter][packed_index] = data_low | (data_high << 30);
                            }
                            break;
                        default:
                            break;
                    } 
                }
            }
        }
    }

    return relin_constants;
}

#endif
