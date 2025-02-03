#include "utils1.h"
#include "Network1.h"
#include <string>
#include <fstream>
#include "syscall.h"
#include "string.h"

#define OCALL_PRINT_TIME 3
#define OCALL_WAIT_FOR_FINISH 8

unsigned long ocall_wait_for_finish(char* string)
{
	unsigned long retval;
	ocall(OCALL_WAIT_FOR_FINISH, string, strlen(string) + 1, &retval, sizeof(unsigned long));
	return retval;
}

unsigned long ocall_time(char* string) 
{
    unsigned long retval;
    ocall(OCALL_PRINT_TIME, string, strlen(string) + 1, &retval, sizeof(unsigned long));
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

void test()
{
    Network network;
    
    ocall_time("Test 1 Start\n");
		network.Forward();
		ocall_wait_for_finish("Test 1 End");
		
		// ocall_time("Test 2 Start\n");
		// network.Forward();
		// ocall_wait_for_finish("Test 2 End");
		
		// ocall_time("Test 3 Start\n");
		// network.Forward();
		
 		return;
}



