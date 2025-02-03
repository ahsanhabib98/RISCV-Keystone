#include "utils2.h"
#include "Network2.h"
#include <string>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include <iostream>

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"
#include "crypto.h"
#include "aes.hpp"

#define OCALL_GET_STRING_12 2
#define OCALL_WAIT_FOR_FINISH 8

unsigned long ocall_wait_for_finish(char* string)
{
	unsigned long retval;
	ocall(OCALL_WAIT_FOR_FINISH, string, strlen(string) + 1, &retval, sizeof(unsigned long));
	return retval;
}

float Accuracy_1(float *pfPred, int *pnLab, int nclass)
{
    float flag = 0.0;
    for (int i = 0; i < nclass; i++)
    {
        //cout << pfPred[i] << ':' << pnLab[i] << endl;
        if (pfPred[i] >=0.5 && pnLab[i] == 1)
            flag = 1.0;
    }
    return flag;
}

float Accuracy_all(float *pfPred, int *pnLab, int nclass)
{
    float flag = 0.0, nSum = 0.0;
    int *pnPred = new int[nclass];
    for (int i = 0; i < nclass; i++)
    {
        pnPred[i] = 0;
        if (pfPred[i] >=0.5)
            pnPred[i] = 1;

        if (pnPred[i] == pnLab[i])
            nSum += 1;
    }

    if (nSum == 12.0)
        flag = 1.0;

    delete[] pnPred;
    return flag;
}

float* getInput(AES_ctx* ctx, size_t inputSize)
{
	float* inputData = (float*)malloc(inputSize * sizeof(float));
	if(inputData == NULL)
	{
		fprintf(stderr, "Mem alloc failed\n");
		return NULL;
	}
	memset(inputData, 0, inputSize * sizeof(float));
	
	size_t index = 0;
	
	while(index < inputSize)
	{
		uint8_t nonce[2048];
		size_t nonceLen = 2048;
		struct edge_data retdata;
		ocall(OCALL_GET_STRING_12, NULL, 0, &retdata, sizeof(struct edge_data));
		if(retdata.size > 2048)
			retdata.size = 2048;
		memset(nonce, 0, sizeof(nonce));
		copy_from_shared(nonce, retdata.offset, retdata.size);
		
		//decrypt
		AES_CBC_decrypt_buffer(ctx, (uint8_t*)nonce, 2048);
		remove_padding((uint8_t*)nonce, &nonceLen);
		
		if(nonceLen > 2032)
			nonceLen = 2032;
		
		for(size_t i = 0; i < nonceLen; i += sizeof(float))
		{
			if(index < inputSize)
			{
				memcpy(&inputData[index], &nonce[i], sizeof(float)); 
				index++;
			}
			else
				break;
		}
	}
	
	return inputData;
}

void test()
{
  Network network;
  size_t inputSize = 275968;
  
  struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
  
	network.Forward(getInput(&ctx, inputSize));
	ocall_wait_for_finish("Test 1\n");
	
	// AES_init_ctx_iv(&ctx, key, iv);
	// network.Forward(getInput(&ctx, inputSize));
	// ocall_wait_for_finish("Test 2\n");
	
	// AES_init_ctx_iv(&ctx, key, iv);
	// network.Forward(getInput(&ctx, inputSize));
	
	return;
}


