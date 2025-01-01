#include "Network2.h"
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

#ifndef OCALL_PRINT_STRING_23
#define OCALL_PRINT_STRING_23 1
#endif

#ifndef OCALL_PRINT_TIME
#define OCALL_PRINT_TIME 3
#endif

#define OCALL_PRINT_BUFFER 6

uint8_t key[AES_KEYLEN] = {
			(uint8_t)0x1a, (uint8_t)0x2b, (uint8_t)0x3b, (uint8_t)0x4d,
			(uint8_t)0x5e, (uint8_t)0x6f, (uint8_t)0x71, (uint8_t)0x82,
			(uint8_t)0x93, (uint8_t)0x14, (uint8_t)0x25, (uint8_t)0x36,
			(uint8_t)0x47, (uint8_t)0x58, (uint8_t)0x69, (uint8_t)0x7a 
	};

extern "C" 
{
  unsigned long ocall_print_string(char* string) 
  {
      unsigned long retval;
      ocall(OCALL_PRINT_STRING_23, string, strlen(string) + 1, &retval, sizeof(unsigned long));
      return retval;
  }
  
  unsigned long ocall_print_buffer(char* string) 
	{
		unsigned long retval;
		ocall(OCALL_PRINT_BUFFER, string, strlen(string) + 1, &retval, sizeof(unsigned long));
		return retval;
	}
  
  unsigned long ocall_print_time(char* string) 
  {
      unsigned long retval;
      ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
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
	ocall_print_time("Network Init 2 Start");
	
	ocall_print_buffer("Initializing Network 2...\n");

	// Second Layer
	m_Layers_ds2_1 = new Layers_Ds(22, 45, 112, 1, 211, 212);  // Reduced from 32 -> 64 to 22 -> 45
	m_Layers_ds2_2 = new Layers_Ds(45, 90, 112, 2, 221, 222);  // Reduced from 64 -> 128 to 45 -> 90

	// Third Layer
	m_Layers_ds3_1 = new Layers_Ds(90, 90, 56, 1, 311, 312);  // Reduced from 128 -> 128 to 90 -> 90
	m_Layers_ds3_2 = new Layers_Ds(90, 180, 56, 2, 321, 322);  // Reduced from 128 -> 256 to 90 -> 180

	// Fourth Layer
	m_Layers_ds4_1 = new Layers_Ds(180, 180, 28, 1, 411, 412);  // Reduced from 256 -> 256 to 180 -> 180
	m_Layers_ds4_2 = new Layers_Ds(180, 360, 28, 2, 421, 422);  // Reduced from 256 -> 512 to 180 -> 360
	
	// Fifth Layer
	m_Layers_ds5_1 = new Layers_Ds(360, 360, 14, 1, 511, 512);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_2 = new Layers_Ds(360, 360, 14, 1, 521, 522);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_3 = new Layers_Ds(360, 360, 14, 1, 531, 532);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_4 = new Layers_Ds(360, 360, 14, 1, 541, 542);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_5 = new Layers_Ds(360, 360, 14, 1, 551, 552);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_6 = new Layers_Ds(360, 720, 14, 2, 561, 562);  // Reduced from 512 -> 1024 to 360 -> 720
	
	// Sixth Layer
	m_Layers_ds6 = new Layers_Ds(720, 720, 7, 1, 61, 62);  // Reduced from 1024 -> 1024 to 720 -> 720

	// Global Pooling Layer
	m_Poollayer6 = new GlobalPoolLayer(720, 7);  // Reduced from 1024 to 720
	
	ocall_print_buffer("Initializing Network 2 Done...\n");
	
	ocall_print_time("Network Init 2 End");
}


Network::~Network()
{
    delete m_Layers_ds2_1;
    delete m_Layers_ds2_2;
    delete m_Layers_ds3_1;
    delete m_Layers_ds3_2;
    delete m_Layers_ds4_1;
    delete m_Layers_ds4_2;
    
    delete m_Layers_ds5_1;
    delete m_Layers_ds5_2;
    delete m_Layers_ds5_3;
    delete m_Layers_ds5_4;
    delete m_Layers_ds5_5;
    delete m_Layers_ds5_6;
    delete m_Layers_ds6;
    
    delete m_Poollayer6;
}

void Network::Forward(float* input)
{
	ocall_print_time("Inference 2 Start");

	m_Layers_ds2_1->forward(input);
	m_Layers_ds2_2->forward(m_Layers_ds2_1->GetOutput());

	m_Layers_ds3_1->forward(m_Layers_ds2_2->GetOutput());
	m_Layers_ds3_2->forward(m_Layers_ds3_1->GetOutput());

	m_Layers_ds4_1->forward(m_Layers_ds3_2->GetOutput());
	m_Layers_ds4_2->forward(m_Layers_ds4_1->GetOutput());
	
 	m_Layers_ds5_1->forward(m_Layers_ds4_2->GetOutput());
  m_Layers_ds5_2->forward(m_Layers_ds5_1->GetOutput());
  m_Layers_ds5_3->forward(m_Layers_ds5_2->GetOutput());
  m_Layers_ds5_4->forward(m_Layers_ds5_3->GetOutput());
  m_Layers_ds5_5->forward(m_Layers_ds5_4->GetOutput());
  m_Layers_ds5_6->forward(m_Layers_ds5_5->GetOutput());
	m_Layers_ds5_6->forward(m_Layers_ds5_5->GetOutput());
	
  m_Layers_ds6->forward(m_Layers_ds5_6->GetOutput());

  m_Poollayer6->forward(m_Layers_ds6->GetOutput());
  
  m_pfOutput = m_Poollayer6->GetOutput();
	
	ocall_print_time("Inference 2 End");
	
	// struct AES_ctx ctx;
	// AES_init_ctx_iv(&ctx, key, iv);

	ocall_print_time("Communication 2 Start");
	
	size_t bytesToWrite = 720 * sizeof(float);
	
	uint8_t inputBuf[2032];
	size_t offset = 0;
	
	while(bytesToWrite > 0) 
	{
		//write 2032 bytes or whatever is left
		size_t chunkSize = (bytesToWrite > 2032) ? 2032 : bytesToWrite;
		
		memcpy(inputBuf, reinterpret_cast<uint8_t*>(m_pfOutput) + offset, chunkSize);
		
		//apply padding
		size_t length = sizeof(inputBuf);
		uint8_t* buffer = (uint8_t*)malloc(length + AES_BLOCKLEN);
		memcpy(buffer, inputBuf, length);
		size_t padded_length = length;
		pad_buffer(buffer, &padded_length);
		
		//encrypt
		// AES_CBC_encrypt_buffer(&ctx, buffer, padded_length);
		ocall_print_string((char*)buffer);
		
		offset += chunkSize;
		bytesToWrite -= chunkSize;
		free(buffer);
	}
	
	ocall_print_time("Communication 2 End");
	
	return;
}
