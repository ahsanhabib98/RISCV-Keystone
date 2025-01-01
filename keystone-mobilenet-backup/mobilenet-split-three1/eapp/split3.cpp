#include "utils3.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include <iostream>

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"
#include "aes.hpp"
#include "rsa.cpp"

#define OCALL_PRINT_TIME 3
#define OCALL_SEND_REPORT 4
#define OCALL_GET_NONCE 5
#define OCALL_WAIT_FOR_PUBLIC_KEY 7
#define OCALL_WAIT_FOR_KEY_ACKNOWLEDGE 8

// Original data
uint8_t key_data[150] = {0xbc, 0xd8, 0xb9, 0x11, 0x5b, 0x57, 0xc6, 0x8f, 0x90, 0xc2, 0xed, 0x97, 0x62, 0x84, 0x2e, 0x21, 0x99, 0x4c, 0xb0, 0x2d,
                      0xe5, 0x75, 0x9f, 0x87, 0x38, 0x23, 0xad, 0xa4, 0x74, 0xdb, 0x16, 0x5a, 0x29, 0x39, 0xd8, 0xad, 0x21, 0xcb, 0x9c, 0x7b,
                      0xbc, 0x99, 0xc2, 0x83, 0x5e, 0x0d, 0x7c, 0xd6, 0xc5, 0x29, 0xd2, 0xd0, 0x71, 0xf6, 0xa5, 0x42, 0xc9, 0xe0, 0x5c, 0x5c,
                      0xe2, 0xa3, 0x91, 0x9b, 0x1a, 0x2d, 0x60, 0x14, 0x0b, 0x7c, 0x0a, 0xfd, 0x54, 0x5f, 0xc7, 0xc1, 0x0c, 0xeb, 0xe9, 0x59,
                      0x23, 0x51, 0xf0, 0x3e, 0x95, 0x8f, 0xcf, 0xf6, 0x43, 0xcc, 0x08, 0xf4, 0x58, 0x62, 0xcc, 0xe9, 0x49, 0x6a, 0x46, 0xb6,
                      0x5a, 0x72, 0xb4, 0x0c, 0x38, 0xf0, 0xc0, 0x82, 0xd7, 0x2e, 0xf9, 0x9e, 0x97, 0x2d, 0xe6, 0xee, 0xa9, 0xb9, 0xe0, 0xda,
                      0x9d, 0xaa, 0xe3, 0xd1, 0x32, 0xd9, 0xea, 0xf9};


unsigned long ocall_print_time(char* string) 
{
    unsigned long retval;
    ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
    return retval;
}

int main()
{
	ocall_print_time("Enclave3 Start");

  ocall_print_time("Remote Attestation 3 Start");
  struct edge_data retdata;
  ocall(OCALL_GET_NONCE, NULL, 0, &retdata, sizeof(struct edge_data));

  char nonce[2048];
  if (retdata.size > 2048) retdata.size = 2048;
  copy_from_shared(nonce, retdata.offset, retdata.size);

  char buffer[2048];
  attest_enclave((void*)buffer, nonce, retdata.size);

  ocall(OCALL_SEND_REPORT, buffer, 2048, 0, 0);

  struct PublicKey {
        int e;
        int n;
    };
  PublicKey public_key;

  ocall(OCALL_WAIT_FOR_PUBLIC_KEY, nullptr, 0, &retdata, sizeof(struct edge_data));
  copy_from_shared(&public_key, retdata.offset, retdata.size);
  
  int e = public_key.e;
  int n = public_key.n;

  // Encrypt the key data
  int key_data_length = sizeof(key_data) / sizeof(key_data[0]);
  int* encrypted_key_data = encrypt(key_data, key_data_length, e, n);

  // Serialize parameters
  struct KeyParameters {
      int encrypted_key_size;
      int encrypted_key[1024];
  };

  KeyParameters params;
  params.encrypted_key_size = key_data_length;
  memcpy(params.encrypted_key, encrypted_key_data, key_data_length * sizeof(int));

  ocall(OCALL_WAIT_FOR_KEY_ACKNOWLEDGE, &params, sizeof(KeyParameters), NULL, 0);
  ocall_print_time("Remote Attestation 3 End");
	
	test();
	
  ocall_print_time("Enclave3 End");
	return 0;
}
