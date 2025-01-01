#ifndef NETWORK_H
#define NETWORK_H

#include "convLayer2.h"
#include "globalpoolLayer.h"
#include "batchnormalLayer2.h"
#include "reluLayer.h"
#include "fcLayer.h"
#include "sigmoidLayer.h"
#include "layers_bn2.h"
#include "layers_ds.h"
#include <vector>
#include <iostream>
#include "string.h"

using namespace std;

class Network
{
public:
    Network();
    ~Network();

    float* Forward(float* input);
    

private:
    //float *m_pfOutput;
    //float m_dAccuracy;
    //int m_nPrediction;
    vector <const char *> m_vcClass;
    const char *m_cClass[10];

    FcLayer *m_Fclayer7;

    SigmoidLayer *m_Sigmoidlayer8;
};


#endif
