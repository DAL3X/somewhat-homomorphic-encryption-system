#ifndef PARAMETERS
#define PARAMETERS

#include <inttypes.h>

#define NUMPRIMES 6             // Number of used co-primes for RNS.
#define POLY_DEG 4096           // Number of coefficient per polynomial.
#define PACKED_POLY_DEG 2048    // Number of coefficients per packed polynomial.
#define PLAIN_PRIME 28          // Number bits for the plaintext mod. Has to be smaller than 30.

// NUMPRIMES x 30-bit prime numbers + one special prime and one switching prime for modulo switching.
uint64_t primes[NUMPRIMES] = {1068564481 , 1069219841, 1070727169, 1071513601, 1072496641, 1073479681};
uint64_t special_prime = 1068433409;
uint64_t switching_prime = 1068236801;

#endif
