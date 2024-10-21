//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "eapp_utils.h"
#include "syscall.h"
#include "edge/edge_common.h"
#include "string.h"

#include "utils1.h"
#include "aes.hpp"
#include "crypto.h"

#define OCALL_PRINT_TIME 3
#define OCALL_SEND_REPORT 4
#define OCALL_GET_NONCE 5
#define OCALL_WAIT_FOR_KEY 7

uint8_t key[AES_BLOCKLEN] = { 0 };

unsigned long ocall_print_time(char* string) 
{
    unsigned long retval;
    ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
    return retval;
}

int
main() {
	ocall_print_time("Enclave1 Start");
	
	ocall_print_time("Remote Attestation Start");
  struct edge_data retdata;
  ocall(OCALL_GET_NONCE, NULL, 0, &retdata, sizeof(struct edge_data));

  char nonce[2048];
  if (retdata.size > 2048) retdata.size = 2048;
  copy_from_shared(nonce, retdata.offset, retdata.size);

  char buffer[2048];
  attest_enclave((void*)buffer, nonce, retdata.size);

  ocall(OCALL_SEND_REPORT, buffer, 2048, 0, 0);
	ocall(OCALL_WAIT_FOR_KEY, NULL, 0, key, AES_KEYLEN);
  ocall_print_time("Remote Attestation End");
  
  test();
  
  ocall_print_time("Enclave1 End");

  EAPP_RETURN(0);
}


