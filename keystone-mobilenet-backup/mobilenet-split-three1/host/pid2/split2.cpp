#include "utils2.cpp"
// #include "../host.cpp"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include <iostream>

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.c"

#define OCALL_PRINT_TIME 3

unsigned long ocall_print_time2(const char* string) 
{
    unsigned long retval;
    ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
    return retval;
    //  return print_time_wrapper(const_cast<char*>(string));
}

char* pid2main(char * var, int length)
{
  cout << "Ocall Start " <<endl;
	//ocall_print_time2("Enclave2 Start");
  cout << "Ocall finish " <<endl;
	
	return test(var, length);
	
  //ocall_print_time2("Enclave2 End");
	//return 0;
}
