#include <edge_call.h>
#include <host/keystone.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <chrono>
#include <iomanip>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "verifier/report.h"
#include "verifier/test_dev_key.h"
#include "enclave_expected_hash.h"
#include "sm_expected_hash.h"

#include "aes.hpp"
#include "pid2/Network2.cpp"

#include "rsa.cpp"
#include "rsa.h"
#include <algorithm>

using namespace std;

void concatStrings(char* dest, char* concat);
void send_input_data();
void print_destroy_time(int num);
unsigned long print_buffer(char* str);
unsigned long print_string(char* str);
unsigned long print_time(char* str);
void print_string_wrapper12(void* buffer);
float* get_host_string_wrapper12(size_t inputSize);
void print_string_wrapper23(float* network2_output);
void get_host_string_wrapper23(void* buffer);
void print_time_wrapper(void* buffer);
void get_host_string_wrapper01(void* buffer);

void send_report_wrapper(void* buffer);
void get_nonce_wrapper(void* buffer);
void wait_for_public_key_wrapper(void* buffer);
void wait_for_key_acknowledge_wrapper(void* buffer);
void print_buffer_wrapper(void* buffer);

void verify_report(void* buffer);

void wait_for_finish_wrapper(void* buffer);
void finish_wrapper(void* buffer);
void print_output_wrapper(void* buffer);

void request_input_wrapper(void* buffer);

#define AES_BLOCKLEN 16 //block size for AES
void pad_buffer(uint8_t* buf, size_t* length);
void remove_padding(uint8_t* buf, size_t* length);

void generate_keys(int &e, int &d, int &n);

uint8_t key[AES_KEYLEN] = {
			(uint8_t)0x1a, (uint8_t)0x2b, (uint8_t)0x3b, (uint8_t)0x4d,
			(uint8_t)0x5e, (uint8_t)0x6f, (uint8_t)0x71, (uint8_t)0x82,
			(uint8_t)0x93, (uint8_t)0x14, (uint8_t)0x25, (uint8_t)0x36,
			(uint8_t)0x47, (uint8_t)0x58, (uint8_t)0x69, (uint8_t)0x7a 
		};
		
uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

// Public Key (e, n) and Private Key (d, n)
int e, d, n;

#define OCALL_PRINT_STRING_12 1
#define OCALL_GET_STRING_12 2

#define OCALL_PRINT_STRING_23 1
#define OCALL_GET_STRING_23 2

#define OCALL_GET_STRING_01 2

#define OCALL_PRINT_TIME 3
#define OCALL_SEND_REPORT 4
#define OCALL_GET_NONCE 5
#define OCALL_PRINT_BUFFER 6
#define OCALL_WAIT_FOR_PUBLIC_KEY 7
#define OCALL_WAIT_FOR_KEY_ACKNOWLEDGE 8

#define OCALL_WAIT_FOR_FINISH 9
#define OCALL_FINISH 9

#define OCALL_REQUEST_INPUT 10
#define OCALL_PRINT_OUTPUT 10

void print_hex(const uint8_t *data, size_t len);

void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

unsigned long print_buffer(char* str){
  printf("%s",str);
  return strlen(str);
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

unsigned long print_string(char* str) //dont actually print
{
  return strlen(str);
}

unsigned long print_time(char* str)
{
	// Get the current time point from the system clock
  auto now = std::chrono::system_clock::now();
  
  // Convert the time point to a time_t which represents the time in seconds
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  
  // Convert the time_t to a tm struct for formatting
  std::tm* now_tm = std::localtime(&now_time_t);
  
  // Get the number of milliseconds since the last second
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // Print the time with millisecond precision
  std::cout << "Time: ";
  std::cout << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
  std::cout << '.' << std::setfill('0') << std::setw(3) << milliseconds.count() << " : ";
	return printf("%s\n", str); 
}

char var[2048];
int encrypted_var[2048];
int inputPipefd[2];
int pipefd[2];
int attestPipefd[2];
sem_t* sent12;
sem_t* received12;
sem_t* sent23;
sem_t* received23;
sem_t* sent01;
sem_t* received01;
sem_t* verified;
sem_t* encrypted;
sem_t* finished;

char attestNonce[2048];

int main(int argc, char** argv) 
{
  print_time("Host Init Start"); 	
  auto start = std::chrono::high_resolution_clock::now();
	
	// Get the current time point from the system clock
  auto now = std::chrono::system_clock::now();
  
  // Convert the time point to a time_t which represents the time in seconds
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  
  // Convert the time_t to a tm struct for formatting
  std::tm* now_tm = std::localtime(&now_time_t);
  
  // Get the number of milliseconds since the last second
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // Print the time with millisecond precision
  std::cout << "Start Time: ";
  std::cout << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
  std::cout << '.' << std::setfill('0') << std::setw(3) << milliseconds.count() << std::endl;

	sent12 = sem_open("/sent12", O_CREAT, 0644, 0);
	received12 = sem_open("/received12", O_CREAT, 0644, 1);
	sent23 = sem_open("/sent23", O_CREAT, 0644, 0);
	received23 = sem_open("/received23", O_CREAT, 0644, 1);
	verified = sem_open("/verified", O_CREAT, 0644, 0);
	encrypted = sem_open("/encrypted", O_CREAT, 0644, 0);
	finished = sem_open("/finished", O_CREAT, 0644, 0);

	pipe(pipefd);
	pipe(inputPipefd);
	pipe(attestPipefd);
	pid_t pid1, pid2, pid3;
    
  pid1 = fork();
  if(pid1 == 0)
  {
    // First child process
    Keystone::Enclave enclave;
    Keystone::Params params;  
    params.setFreeMemSize(8000 * 8000);
    params.setUntrustedMem(DEFAULT_UNTRUSTED_PTR, 1024 * 1024);
    enclave.init(argv[1], argv[4], params);
    enclave.registerOcallDispatch(incoming_call_dispatch);
    register_call(OCALL_GET_STRING_01, get_host_string_wrapper01);
    register_call(OCALL_PRINT_STRING_12, print_string_wrapper12);
    register_call(OCALL_PRINT_TIME, print_time_wrapper);
    register_call(OCALL_SEND_REPORT, send_report_wrapper);
    register_call(OCALL_GET_NONCE, get_nonce_wrapper);
    register_call(OCALL_PRINT_BUFFER, print_buffer_wrapper);
    register_call(OCALL_WAIT_FOR_PUBLIC_KEY, wait_for_public_key_wrapper);
    register_call(OCALL_WAIT_FOR_KEY_ACKNOWLEDGE, wait_for_key_acknowledge_wrapper);
    register_call(OCALL_WAIT_FOR_FINISH, wait_for_finish_wrapper);
    register_call(OCALL_REQUEST_INPUT, request_input_wrapper);
    edge_call_init_internals((uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());
    enclave.run();
    print_destroy_time(1);
    return 0; // Child process should exit after running the enclave
  }
  
  pid2 = fork();
  if(pid2 == 0)
  {
    // Second child process
    Network network2;
    size_t inputSize = 100352;
	float* inputData = get_host_string_wrapper12(inputSize);
	float* network2_output = network2.Forward(inputData);
	free(inputData);
	print_time("Communication 2 Start");
	print_string_wrapper23(network2_output);
	print_time("Communication 2 End");
    return 0; // Child process should exit after running the process
  }

  pid3 = fork();
  if(pid3 == 0)
  {
    // Third child process
    Keystone::Enclave enclave;
    Keystone::Params params;
    params.setFreeMemSize(2048 * 2048);
    params.setUntrustedMem(DEFAULT_UNTRUSTED_PTR, 1024 * 1024);
    enclave.init(argv[3], argv[4], params);
    enclave.registerOcallDispatch(incoming_call_dispatch);
    register_call(OCALL_GET_STRING_23, get_host_string_wrapper23);
    register_call(OCALL_PRINT_TIME, print_time_wrapper);
    register_call(OCALL_PRINT_BUFFER, print_buffer_wrapper);
    register_call(OCALL_PRINT_OUTPUT, print_output_wrapper);
    register_call(OCALL_FINISH, finish_wrapper);
    edge_call_init_internals((uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());
    enclave.run();
    print_destroy_time(3);
    return 0; // Child process should exit after running the enclave
  }
	
	if(pid1 != 0 && pid2 != 0 && pid3 != 0)
  {
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
		
		close(pipefd[0]);
		close(pipefd[1]);
		close(inputPipefd[0]);
		close(inputPipefd[1]);
		
		sem_close(sent12);
    sem_close(received12);
    sem_close(sent23);
    sem_close(received23);
    sem_close(verified);
    sem_close(encrypted);
    sem_close(finished);
    sem_unlink("/sent12");
    sem_unlink("/received12");
    sem_unlink("/sent23");
    sem_unlink("/received23");
    sem_unlink("/verified");
    sem_unlink("/encrypted");
    sem_unlink("/finished");
		
		auto end = std::chrono::high_resolution_clock::now();
		auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		double seconds = time_taken.count() / 1000.0;
		printf("Host Execution Time: %.6f seconds\n", seconds);
	}
  return 0;
}

void send_input_data() {
    std::cout << "Reading input data" << std::endl;

    // Construct the filename for the image
    std::string filename = "2.png";

    // Variables to hold image width, height, and number of channels
    int width, height, channels;

    // Load the image
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    std::cout << width << std::endl;
    std::cout << height << std::endl;
    std::cout << channels << std::endl;

    // Check if the image was loaded correctly
    if (image == nullptr) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return;
    }

    // Calculate the total size of the image (width * height * channels)
    size_t inputSize = width * height * channels;

    // Create a float array to hold the pixel values
    float* inputData = new float[inputSize];

    // Convert the pixel values from unsigned char to float
    for (size_t j = 0; j < inputSize; ++j) {
        inputData[j] = static_cast<float>(image[j]);
    }

    // Free the image memory
    stbi_image_free(image);

	print_time("Host Init End");

    // Encryption and sending logic
    std::cout << "Sending input..." << std::endl;
    size_t bytesToWrite = inputSize * sizeof(float);
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);

    uint8_t inputBuf[2032];
    size_t offset = 0;

    while (bytesToWrite > 0) {
        // Write 2032 bytes or whatever is left
        size_t chunkSize = (bytesToWrite > 2032) ? 2032 : bytesToWrite;

        // Copy a chunk of the input data to the buffer
        memcpy(inputBuf, reinterpret_cast<const uint8_t*>(inputData) + offset, chunkSize);

        // Apply padding
        size_t length = chunkSize; // Correct the length to chunk size
        uint8_t* buffer = (uint8_t*)malloc(length + AES_BLOCKLEN);
        memcpy(buffer, inputBuf, length);
        size_t padded_length = length;
        pad_buffer(buffer, &padded_length);

        // Encrypt
        AES_CBC_encrypt_buffer(&ctx, buffer, padded_length);
        write(inputPipefd[1], buffer, padded_length);

        offset += chunkSize;
        bytesToWrite -= chunkSize;
        free(buffer);
    }

    // Clean up the allocated float array
    delete[] inputData;

    std::cout << "Input data sent" << std::endl;
}


void print_destroy_time(int num)
{
	std::cout << "Enclave " << num << " destroyed at ";
	// Get the current time point from the system clock
	auto now = std::chrono::system_clock::now();

	// Convert the time point to a time_t which represents the time in seconds
	auto now_time_t = std::chrono::system_clock::to_time_t(now);

	// Convert the time_t to a tm struct for formatting
	auto now_tm = std::localtime(&now_time_t);

	// Get the number of milliseconds since the last second
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	// Print the time with millisecond precision
	std::cout << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
	std::cout << '.' << std::setfill('0') << std::setw(3) << milliseconds.count() << std::endl;
}

void
print_string_wrapper12(void* buffer) 
{
	//cout << "Waiting for receive" << endl;
	sem_wait(received12);
	
	/* Parse and validate the incoming call data */
	struct edge_call* edge_call = (struct edge_call*)buffer;
	uintptr_t call_args;
	unsigned long ret_val;
	size_t arg_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}  /* Pass the arguments from the eapp to the exported ocall function */
	
	strcpy(var,(char*)call_args);

	write(pipefd[1],var,sizeof(var));
	
	sem_post(sent12);
	return;
}

float*
get_host_string_wrapper12(size_t inputSize) 
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
		sem_wait(sent12);
		uint8_t buff[2048];
		ssize_t bytes_read = read(pipefd[0], buff, sizeof(buff));
		
		for(size_t i = 0; i < bytes_read; i += sizeof(float))
		{
			if(index < inputSize)
			{
				memcpy(&inputData[index], &buff[i], sizeof(float)); 
				index++;
			}
			else
				break;
		}
		sem_post(received12);
	}
	return inputData;
}

void
print_string_wrapper23(float* network2_output) 
{
	size_t bytesToWrite = 720 * sizeof(float);
	
	uint8_t inputBuf[2032];
	size_t offset = 0;

	while(bytesToWrite > 0) 
	{
		sem_wait(received23);
		//write 2032 bytes or whatever is left
		size_t chunkSize = (bytesToWrite > 2032) ? 2032 : bytesToWrite;
		
		memcpy(inputBuf, reinterpret_cast<uint8_t*>(network2_output) + offset, chunkSize);
		
		//apply padding
		size_t length = sizeof(inputBuf);
		uint8_t* buffer = (uint8_t*)malloc(length + AES_BLOCKLEN);
		memcpy(buffer, inputBuf, length);
		size_t padded_length = length;
		pad_buffer(buffer, &padded_length);
		
		write(pipefd[1], buffer, sizeof(buffer));

		sem_post(sent23);
		
		offset += chunkSize;
		bytesToWrite -= chunkSize;
		free(buffer);
	}
}

void
get_host_string_wrapper23(void* buffer) 
{
	//cout << "Waiting for sent" << endl;
	sem_wait(sent23);
	
	/* For now we assume the call struct is at the front of the shared
	* buffer. This will have to change to allow nested calls. */
	struct edge_call* edge_call = (struct edge_call*)buffer;  
	uintptr_t call_args;
	unsigned long ret_val;
	size_t args_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
	
	//close(pipefd[1]);
	uint8_t buff[sizeof(var)];
	ssize_t bytes_read = read(pipefd[0],buff,sizeof(buff));
	
	const uint8_t* host_str=buff;
	size_t host_str_len  = bytes_read;  // This handles wrapping the data into an edge_data_t and storing it
	// in the shared region.
	
	if (edge_call_setup_wrapped_ret(edge_call, (void*)host_str, host_str_len)) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	} 
	else 
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	//cout << "Received" << endl;
	sem_post(received23);
	return;
}

void
print_time_wrapper(void* buffer) 
{
	/* Parse and validate the incoming call data */
	struct edge_call* edge_call = (struct edge_call*)buffer;
	uintptr_t call_args;
	unsigned long ret_val;
	size_t arg_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
	
	ret_val = print_time((char*)call_args);
	
	/* Setup return data from the ocall function */
	uintptr_t data_section = edge_call_data_ptr();
	memcpy((void*)data_section, &ret_val, sizeof(unsigned long));
	if (edge_call_setup_ret(edge_call, (void*)data_section, sizeof(unsigned long))) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	} 
	else 
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	/* This will now eventually return control to the enclave */
	return;
}

void
get_host_string_wrapper01(void* buffer) 
{
	//cout << "Waiting for sent" << endl;
	//sem_wait(sent01);
	
	/* For now we assume the call struct is at the front of the shared
	* buffer. This will have to change to allow nested calls. */
	struct edge_call* edge_call = (struct edge_call*)buffer;  
	uintptr_t call_args;
	unsigned long ret_val;
	size_t args_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
	
	//close(pipefd[1]);
	uint8_t buff[sizeof(var)];
	ssize_t bytes_read = read(inputPipefd[0], buff, sizeof(buff));
	
	const uint8_t* host_str = buff;
	size_t host_str_len  = bytes_read;  // This handles wrapping the data into an edge_data_t and storing it
	// in the shared region.
	
	if (edge_call_setup_wrapped_ret(edge_call, (void*)host_str, host_str_len)) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	} 
	else 
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	//cout << "Received" << endl;
	//sem_post(received01);
	return;
}

void send_report_wrapper(void* buffer)
{
	struct edge_call* edge_call = (struct edge_call*)buffer;
	
	uintptr_t data_section;
	unsigned long ret_val;
	
	if(edge_call_get_ptr_from_offset(edge_call->call_arg_offset, sizeof(report_t), &data_section) != 0)
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
	
	verify_report((void*)data_section);
	
	edge_call->return_data.call_status = CALL_STATUS_OK;
	
}

void verify_report(void* buffer)
{
	//verify report
	Report report;
	report.fromBytes((unsigned char*)buffer);
	report.printPretty();
	
	bool valid = true;
	
	if(report.verify(enclave_expected_hash, sm_expected_hash, _sanctum_dev_public_key))
	{
		printf("Attestation signature and enclave hash are valid\n");
	}
	else
	{
		printf("Attestation report is NOT valid\n");
		valid = false;
	}
	
	if(report.getDataSize() != strlen(attestNonce) + 1)
	{
		valid = false;
		const char error[] = "The size of the data in the report is not equal to the size of the nonce initially sent";
		printf(error);
		report.printPretty();
		throw std::runtime_error(error);
	}
	
	if( 0 == memcmp(attestNonce, (char*)report.getDataSection(), strlen(attestNonce)))
	{
		printf("Returned data in the report match with the nonce sent.\n");
	}
	else
	{
		printf("Returned data in the report does NOT match with the nonce sent.\n");
		valid = false;
	}
	
	if(valid || !valid)
	{
		sem_post(verified);
	}
	else
	{
		printf("Attestation failed.\n");
	}
}

void get_nonce_wrapper(void* buffer)
{
	snprintf(attestNonce, sizeof(attestNonce), "%u", random() % 0x100000000);
	
	/* For now we assume the call struct is at the front of the shared
	* buffer. This will have to change to allow nested calls. */
	struct edge_call* edge_call = (struct edge_call*)buffer;  uintptr_t call_args;
	unsigned long ret_val;
	size_t args_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
	
	const char* host_str = attestNonce;
	size_t host_str_len  = strlen(host_str) + 1;  // This handles wrapping the data into an edge_data_t and storing it
	// in the shared region.
	
	if (edge_call_setup_wrapped_ret(edge_call, (void*)host_str, host_str_len)) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	} 
	else 
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	return;
}

void print_buffer_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  size_t args_len;
  if(edge_call_args_ptr(edge_call, &call_args, &args_len) != 0){
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  ret_val = print_buffer((char*)call_args);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if( edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))){
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  }
  else{
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void wait_for_public_key_wrapper(void* buffer)
{
	sem_wait(verified);

	struct edge_call* edge_call = (struct edge_call*)buffer;  
	uintptr_t call_args;
	unsigned long ret_val;
	size_t args_len;
	
	if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
		return;
	}
		
	struct PublicKey {
		int e;
		int n;
	};

	// Public Key (e, n) and Private Key (d, n)
	generate_keys(e, d, n);

	PublicKey public_key;
    public_key.e = e;
    public_key.n = n;
	
	if (edge_call_setup_wrapped_ret(edge_call, (void*)&public_key, sizeof(PublicKey))) 
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	} 
	else 
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}

	return;
}

void wait_for_key_acknowledge_wrapper(void* buffer)
{
	struct edge_call* edge_call = (struct edge_call*)buffer;

	// Extract incoming data
    struct KeyParameters {
        size_t encrypted_key_size;
        int encrypted_key[1024];
    };

    KeyParameters params;
	uintptr_t call_args;
    size_t args_len;

	if (edge_call_args_ptr(edge_call, &call_args, &args_len) != 0) {
        edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
        return;
    }

    memcpy(&params, (void*)call_args, sizeof(KeyParameters));

	// Decrypt the key data
	int key_data_length = params.encrypted_key_size;
    uint8_t* decrypted_key_data = decrypt<uint8_t>(params.encrypted_key, key_data_length, d, n);

    std::cout << "Decryption key successful, acknowledged." << std::endl;
	
	return;
}

void wait_for_finish_wrapper(void* buffer)
{
	sem_wait(finished);

	struct edge_call* edge_call = (struct edge_call*)buffer;
	
	unsigned long ret_val;
	
	uintptr_t data_section = edge_call_data_ptr();
	
	if(edge_call_setup_ret(edge_call, (void*)data_section, sizeof(unsigned long)))
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	}
	else
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	return;
}

void finish_wrapper(void* buffer)
{
	sem_post(finished);
	sem_post(finished);

	struct edge_call* edge_call = (struct edge_call*)buffer;
	
	unsigned long ret_val;
	
	uintptr_t data_section = edge_call_data_ptr();
	
	if(edge_call_setup_ret(edge_call, (void*)data_section, sizeof(unsigned long)))
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	}
	else
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	return;
}

void print_output_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call* edge_call = (struct edge_call*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  size_t args_len;
  if(edge_call_args_ptr(edge_call, &call_args, &args_len) != 0){
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  
	//decrypt
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	
	AES_CBC_decrypt_buffer(&ctx, (uint8_t*)call_args, args_len);
	remove_padding((uint8_t*)call_args, &args_len);
  
  ret_val = print_buffer((char*)call_args);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));

  if( edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))){
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  }
  else{
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}

void request_input_wrapper(void* buffer)
{
	struct edge_call* edge_call = (struct edge_call*)buffer;
	
	unsigned long ret_val;
	
	uintptr_t data_section = edge_call_data_ptr();
	
	if(edge_call_setup_ret(edge_call, (void*)data_section, sizeof(unsigned long)))
	{
		edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
	}
	else
	{
		edge_call->return_data.call_status = CALL_STATUS_OK;
	}
	
	pid_t inputPid = fork();
	if(inputPid == 0)
	{
		send_input_data();
		_exit(0);
	}
	
	return;
}

