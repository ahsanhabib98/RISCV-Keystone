#include "utils2.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include <iostream>

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"

#define OCALL_PRINT_TIME 3

unsigned long ocall_print_time(char* string) 
{
    unsigned long retval;
    ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
    return retval;
}

int main()
{
	ocall_print_time("Enclave2 Start");
	
	test();
	
  ocall_print_time("Enclave2 End");
	return 0;
}
