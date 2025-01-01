// rsa.h
#ifndef RSA_H
#define RSA_H

#include <vector>
#include <string>

// Function declarations
int gcd(int a, int b);
bool is_prime(int n);
int generate_prime(int lower, int upper);
int modinv(int a, int m);
int mod_pow(int base, int exponent, int modulus);
void generate_keys(int &e, int &d, int &n);
template <typename T>
int* encrypt(const T* data, int length, int e, int n);
template <typename T>
T* decrypt(const int* encrypted_data, int length, int d, int n);

#endif // RSA_H
