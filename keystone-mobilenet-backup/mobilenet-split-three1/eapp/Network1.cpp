#include "Network1.h"
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

#include "rsa.h"

#define OCALL_PRINT_STRING_12 1
#define OCALL_PRINT_TIME 3
#define OCALL_PRINT_BUFFER 6
#define OCALL_WAIT_FOR_KEY_ACKNOWLEDGE 8

extern "C" 
{

  unsigned long ocall_print_string(char* string) 
  {
      unsigned long retval;
      ocall(OCALL_PRINT_STRING_12, string, strlen(string) + 1, &retval, sizeof(unsigned long));
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
	ocall_print_time("Network Init 1 Start");
	
	ocall_print_buffer("Initializing Network 1...\n");
	
	m_Readdata = new ReadData(1, 224, 224, 3);
	
	// First Layer
	m_Layers_bn = new Layers_Bn(3, 22, 224, 2, 1);  // Reduced from 32 to 22 (32 / sqrt(2))
	
	ocall_print_buffer("Initializing Network 1 Done...\n");
	
	ocall_print_time("Network Init 1 End");
	
	int outputSize = m_Layers_bn->GetOutputSize();
	char print[32];
	sprintf(print, "Network1 Output Size: %d\n", outputSize);
	ocall_print_buffer(print);
}


Network::~Network()
{
		delete m_Readdata;
    delete m_Layers_bn;
}

void Network::Forward()
{
	ocall_print_time("Inference 1 Start");
	
	ocall_print_time("Communication 0 Start");
	m_Layers_bn->forward(m_Readdata->ReadInput(1));
	ocall_print_time("Communication 0 End");
	
	m_pfOutput = m_Layers_bn->GetOutput();
	
	ocall_print_time("Inference 1 End");
	
	// struct AES_ctx ctx;
	// AES_init_ctx_iv(&ctx, key, iv);

	ocall_print_time("Communication 1 Start");
	
	size_t bytesToWrite = m_Layers_bn->GetOutputSize() * sizeof(float);
	
	uint8_t inputBuf[2048];
	size_t offset = 0;
	
	while(bytesToWrite > 0) 
	{
		//write 2032 bytes or whatever is left
		size_t chunkSize = (bytesToWrite > 2048) ? 2048 : bytesToWrite;
		
		memcpy(inputBuf, reinterpret_cast<uint8_t*>(m_pfOutput) + offset, chunkSize);
		
		//apply padding
		size_t length = sizeof(inputBuf);
		uint8_t* buffer = (uint8_t*)malloc(length + AES_BLOCKLEN);
		memcpy(buffer, inputBuf, length);
		size_t padded_length = length;
		pad_buffer(buffer, &padded_length);

		ocall_print_string((char*)buffer);
		
		offset += chunkSize;
		bytesToWrite -= chunkSize;
		free(buffer);
	}
	
	ocall_print_time("Communication 1 End");
	
	return;
}
