#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include "rsa.h"

// Function to compute the greatest common divisor
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Basic primality test (not suitable for large numbers)
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;

    if ((n % 2 == 0) || (n % 3 == 0)) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if ((n % i == 0) || (n % (i + 2) == 0))
            return false;
    }
    return true;
}

// Generate a random prime number within a given range
int generate_prime(int lower, int upper) {
    int num;
    do {
        num = rand() % (upper - lower + 1) + lower;
    } while (!is_prime(num));
    return num;
}

// Extended Euclidean Algorithm to find modular inverse
int modinv(int a, int m) {
    int m0 = m, t, q;
    int x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    // Apply extended Euclidean Algorithm
    while (a > 1) {
        q = a / m;

        t = m;
        m = a % m;
        a = t;

        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

// Modular exponentiation function
int mod_pow(int base, int exponent, int modulus) {
    if (modulus == 1) return 0;
    int result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent % 2 == 1) // If exponent is odd
            result = (result * base) % modulus;
        exponent = exponent >> 1; // exponent = exponent / 2
        base = (base * base) % modulus;
    }
    return result;
}

// Key generation function
void generate_keys(int &e, int &d, int &n) {
    srand(time(0));

    int p = generate_prime(50, 100);
    int q = generate_prime(50, 100);
    while (q == p)
        q = generate_prime(50, 100);

    n = p * q;
    int phi = (p - 1) * (q - 1);

    e = rand() % (phi - 2) + 2; // e in [2, phi-1]
    while (gcd(e, phi) != 1)
        e = rand() % (phi - 2) + 2;

    d = modinv(e, phi);
}

// General encryption function for strings and binary data
template <typename T>
int* encrypt(const T* data, int length, int e, int n) {
    int* encrypted_data = new int[length];
    for (int i = 0; i < length; i++) {
        encrypted_data[i] = mod_pow(static_cast<int>(data[i]), e, n);
    }
    return encrypted_data;
}

// General decryption function for strings and binary data
template <typename T>
T* decrypt(const int* encrypted_data, int length, int d, int n) {
    T* decrypted_data = new T[length + 1];
    for (int i = 0; i < length; i++) {
        decrypted_data[i] = static_cast<T>(mod_pow(encrypted_data[i], d, n));
    }
    decrypted_data[length] = '\0'; // Null-terminate for strings; ignored for binary data
    return decrypted_data;
}

