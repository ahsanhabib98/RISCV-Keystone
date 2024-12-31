#ifndef NETWORK_H
#define NETWORK_H

#include "readdata1.h"
#include "convLayer.h"
#include "batchnormalLayer.h"
#include "reluLayer.h"
#include "layers_bn.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class Network
{
public:
    Network();
    ~Network();

    void Forward();
    

private:
    float *m_pfOutput;
    //float m_dAccuracy;
    //int m_nPrediction;
    //const char *m_cClass[10];
    ReadData *m_Readdata;
    Layers_Bn *m_Layers_bn;
};


#endif
