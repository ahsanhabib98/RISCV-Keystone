#include "Network3.h"
#include <vector>
#include <iostream>
#include "string.h"

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"

#include <chrono>
#include <iomanip>

#include "crypto.h"
#include "aes.hpp"

#ifndef OCALL_PRINT_TIME
#define OCALL_PRINT_TIME 3
#endif

#define OCALL_PRINT_BUFFER 6
#define OCALL_PRINT_OUTPUT 10

uint8_t key[AES_KEYLEN] = {
			(uint8_t)0x1a, (uint8_t)0x2b, (uint8_t)0x3b, (uint8_t)0x4d,
			(uint8_t)0x5e, (uint8_t)0x6f, (uint8_t)0x71, (uint8_t)0x82,
			(uint8_t)0x93, (uint8_t)0x14, (uint8_t)0x25, (uint8_t)0x36,
			(uint8_t)0x47, (uint8_t)0x58, (uint8_t)0x69, (uint8_t)0x7a 
	};	

extern "C" 
{
	unsigned long ocall_print_output(char* string)
	{
		unsigned long retval;
		ocall(OCALL_PRINT_OUTPUT, string, strlen(string) + 1, &retval, sizeof(unsigned long));
		return retval;
	}

	unsigned long ocall_print_time(char* string) 
	{
		unsigned long retval;
		ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
		return retval;
	}
	
	unsigned long ocall_print_buffer(char* string) 
	{
		unsigned long retval;
		ocall(OCALL_PRINT_BUFFER, string, strlen(string) + 1, &retval, sizeof(unsigned long));
		return retval;
	}

	void concatStrings(char* dest, char* concat) 
	{
    size_t destLen = strlen(dest);
    size_t concatLen = strlen(concat);

    // Ensure there is enough space in dest for concat and null terminator
    if (destLen + concatLen + 1 >= 2048) 
    {
      return;  // Not enough space, return without concatenating
    }

    // Move to the end of dest string
    dest += destLen;

    // Copy the concat string to dest
    while (*concat) 
    {
      *dest = *concat;
      dest++;
      concat++;
    }

    // Add null terminator
    *dest = '\0';
	}	
}

using namespace std;

Network::Network()
{
	ocall_print_time("Network Init 3 Start\n");

	ocall_print_buffer("Initializing Network 3...\n");

	// Fully Connected Layer
	m_Fclayer7 = new FcLayer(7, 256, 12);  // Reduced from 1024 to 720
	
	// Sigmoid Layer
	m_Sigmoidlayer8 = new SigmoidLayer(12);
	
	m_vcClass.push_back("Indoor");
	m_vcClass.push_back("Human Photo");
	m_vcClass.push_back("LDR");
	m_vcClass.push_back("Plant");
	m_vcClass.push_back("Shopping Mall");
	m_vcClass.push_back("Beach");
	m_vcClass.push_back("Reverse Light");
	m_vcClass.push_back("Sunset");
	m_vcClass.push_back("Blue Sky");
	m_vcClass.push_back("Snow");
	m_vcClass.push_back("Night");
	m_vcClass.push_back("Text");
	
	ocall_print_buffer("Initializing Network 3 Done...\n");
	
	ocall_print_time("Network Init 3 End");
}


Network::~Network()
{
    delete m_Fclayer7;
    delete m_Sigmoidlayer8;
}


float* Network::Forward(float* input)
{
	ocall_print_time("Inference 3 Start");
	
	ocall_print_buffer("Getting output...\n");
	
  m_Fclayer7->forward(input);

  m_Sigmoidlayer8->forward(m_Fclayer7->GetOutput());

  float *pfOutput = m_Sigmoidlayer8->GetOutput();
	vector <int> argmax;
	vector <float> Max;

  int nOutputSize = m_Fclayer7->GetOutputSize();
	for (int i = 0; i<nOutputSize; i++)
	{
		if (pfOutput[i] > 0.5)
		{
			argmax.push_back(i);
			Max.push_back(pfOutput[i]);
		}
	}
	ocall_print_time("Inference 3 End");
	
	ocall_print_time("Communication 3 Start");
	
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	
	char outputBuf[396] = { 0 };
	
	for (int i = 0; i < argmax.size(); i++)
	{
		char print[32];
		snprintf(print, sizeof(print), "%10f: %d: %s\n", Max[i], argmax[i], m_vcClass[argmax[i]]);
		concatStrings(outputBuf, print);
	}
	//apply padding
	size_t length = sizeof(outputBuf);
	uint8_t* buffer = (uint8_t*)malloc(length + (AES_BLOCKLEN - (length % AES_BLOCKLEN)));
	memcpy(buffer, outputBuf, length);
	size_t padded_length = length;
	pad_buffer(buffer, &padded_length);
	
	//encrypt
	AES_CBC_encrypt_buffer(&ctx, buffer, padded_length);
	ocall_print_output((char*)buffer);

	ocall_print_time("Communication 3 End");
    
  return pfOutput;
}
