#define _CRT_SECURE_NO_WARNINGS

#include "batchnormalLayer.h"
#include <iostream>
#include <cmath>

using namespace std;

BatchNormalLayer::BatchNormalLayer(int fileNum, int nInputNum, int nInputWidth) :
    m_nInputNum(nInputNum), m_nInputWidth(nInputWidth)
{
    m_nInputSize = m_nInputWidth * m_nInputWidth;
    m_pfOutput = new float[m_nInputNum * m_nInputSize];
    m_pfMean = new float[m_nInputNum];
    m_pfVar = new float[m_nInputNum];
    m_pfFiller = new float[m_nInputNum];
    m_pfBias = new float[m_nInputNum];
    ReadParam(fileNum);
}

BatchNormalLayer::~BatchNormalLayer()
{
    delete[] m_pfOutput;
    delete[] m_pfMean;
    delete[] m_pfVar;
    delete[] m_pfFiller;
    delete[] m_pfBias;
}

void BatchNormalLayer::forward(float *pfInput) 
{
    for (int i = 0; i < m_nInputNum; i++)
    {
        for (int j = 0; j < m_nInputSize; j++)
        {
            int nOutputIndex = i * m_nInputSize + j;

            m_pfOutput[nOutputIndex] = m_pfFiller[i] * ((pfInput[nOutputIndex] - m_pfMean[i])
                / sqrt(m_pfVar[i] + 1e-5)) + m_pfBias[i];
        }
    }
}

void BatchNormalLayer::ReadParam(int fileNum)
{
	int nMsize, nVsize, nFsize, nBsize;
  nMsize = m_nInputNum;
  nVsize = m_nInputNum;
  nFsize = m_nInputNum;
  nBsize = m_nInputNum;

	switch(fileNum)
	{
	case(1):
	m_pfMean = new float[nMsize] {-5.15257e-07, -0.0405053, 0.00492582, 0.0669159, -0.136017, 0.000818472, -0.0590174, 0.0439983, -9.37478e-08, 5.7905e-08, 
-1.47252e-08, 0.13997, 0.0448315, -1.06377e-07, 1.41654e-07, 0.00141437, 0.00469195, -5.83113e-05, -0.0483438, 0.0512685, 
-0.0107327, 0.119756 };
	m_pfVar = new float[nVsize] {7.22756e-07, 13515.8, 845.159, 2829.88, 10725.1, 621.258, 18912.9, 9609.89, 3.94632e-07, 9.67993e-09, 
2.32901e-08, 9920.35, 12564.3, 5.23604e-09, 1.55803e-07, 644.579, 726.55, 500.854, 6289.15, 10729.1, 
1881.94, 9839.35 };
	m_pfFiller = new float[nFsize] {-5.15257e-07, -0.0405053, 0.00492582, 0.0669159, -0.136017, 0.000818472, -0.0590174, 0.0439983, -9.37478e-08, 5.7905e-08, 
-1.47252e-08, 0.13997, 0.0448315, -1.06377e-07, 1.41654e-07, 0.00141437, 0.00469195, -5.83113e-05, -0.0483438, 0.0512685, 
-0.0107327, 0.119756 };
	m_pfBias = new float[nBsize] {-3.64502e-06, 0.682912, 0.416179, 0.443006, 0.273728, 0.432146, 0.753429, 0.47652, 1.19725e-07, -1.21712e-08, 
1.29549e-08, 0.661347, 0.469142, 1.5963e-08, 1.12549e-08, 0.38565, 0.477055, 0.400039, 0.510819, 0.370828, 
-0.283554, 0.270707 };
	break;
	}
}

float *BatchNormalLayer::GetOutput()
{
    return m_pfOutput;
}

int BatchNormalLayer::GetOutputSize()
{
    return m_nInputNum * m_nInputSize;
}
