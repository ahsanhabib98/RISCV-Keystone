#include "crypto.h"
#include "string.h"

uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

void xorEnDecrypt(char* input, const char* key, size_t keysize)
{
	char* strPtr = input;
	size_t inputLength = strlen(strPtr);
	
	for(size_t i = 0; i < inputLength; ++i)
	{
		input[i] = input[i] ^ key[i % keysize];
	}
	
	return;
}

void pad_buffer(uint8_t* buf, size_t* length) 
{
	size_t padding_needed = AES_BLOCKLEN - (*length % AES_BLOCKLEN);
	for(size_t i = *length; i < *length + padding_needed; i++)
	{
		buf[i] = (int8_t)padding_needed;
	}
	
	*length += padding_needed;
}

void remove_padding(uint8_t* buf, size_t* length)
{
	uint8_t padding_len = buf[*length - 1];
	if(padding_len > 0 && padding_len <= AES_BLOCKLEN)
	{
		*length -= padding_len;
	}
}

