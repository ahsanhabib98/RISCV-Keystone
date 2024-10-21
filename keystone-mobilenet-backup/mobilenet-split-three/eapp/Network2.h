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
#include <string>

using namespace std;

class Network
{
public:
    Network();
    ~Network();

    void Forward(float* input);
    

private:
    float *m_pfOutput;
    //float m_dAccuracy;
    //int m_nPrediction;
    //const char *m_cClass[10];
    
    Layers_Ds *m_Layers_ds2_1, *m_Layers_ds2_2, *m_Layers_ds3_1, *m_Layers_ds3_2, *m_Layers_ds4_1, *m_Layers_ds4_2, *m_Layers_ds5_1, *m_Layers_ds5_2, *m_Layers_ds5_3, *m_Layers_ds5_4, *m_Layers_ds5_5, *m_Layers_ds5_6, *m_Layers_ds6;

    GlobalPoolLayer *m_Poollayer6;
};


#endif
