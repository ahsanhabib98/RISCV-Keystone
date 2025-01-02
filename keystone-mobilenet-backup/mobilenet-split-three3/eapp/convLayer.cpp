#define _CRT_SECURE_NO_WARNINGS

#include "convLayer.h"
#include <iostream>

using namespace std;

ConvLayer::ConvLayer(int weightName, int nInputNum, int nOutputNum, int nInputWidth, int nKernelWidth, int nPad, int nStride, int nGroup, int biasName) :
    m_nInputNum(nInputNum), m_nOutputNum(nOutputNum), m_nInputWidth(nInputWidth),
    m_nKernelWidth(nKernelWidth), m_nPad(nPad), m_nStride(nStride), m_nGroup(nGroup)
{
    m_nKernelSize = m_nKernelWidth * m_nKernelWidth;
    m_nInputSize = m_nInputWidth * m_nInputWidth;
    m_nInputPadWidth = m_nInputWidth + 2 * m_nPad;
    m_nInputPadSize = m_nInputPadWidth * m_nInputPadWidth;
    m_nOutputWidth = int((m_nInputPadWidth - m_nKernelWidth) / m_nStride + 1);
    m_nOutputSize = m_nOutputWidth * m_nOutputWidth;
    
	m_pfInputPad = new float[m_nInputNum * m_nInputPadWidth * m_nInputPadWidth];

    m_nInputGroupNum = m_nInputNum / m_nGroup;
    m_nOutputGroupNum = m_nOutputNum / m_nGroup;

    m_pfWeight = new float[m_nOutputNum * m_nInputGroupNum * m_nKernelSize];

    if (m_pcBname != NULL)
        m_pfBias = new float[m_nOutputNum];
    m_pfOutput = new float[m_nOutputNum * m_nOutputSize];

	switch(weightName)
	{
		case(1):
			ReadConvWb1();
			break;
	}
}

ConvLayer::~ConvLayer()
{
    delete[] m_pfOutput;
    if (m_pcBname != NULL)
        delete[] m_pfBias;
    delete[] m_pfWeight;
	delete[] m_pfInputPad;
}

void ConvLayer::forward(float *pfInput)
{
    Addpad(pfInput);

    for (int g = 0; g < m_nGroup; g++)
    {
        for (int nOutmapIndex = 0; nOutmapIndex < m_nOutputGroupNum; nOutmapIndex++)
        {
            for (int i = 0; i < m_nOutputWidth; i++)
            {
                for (int j = 0; j < m_nOutputWidth; j++)
                {
                    float fSum = 0;
                    int nInputIndex, nOutputIndex, nKernelIndex, nInputIndexStart, nKernelStart;
                    nOutputIndex = g * m_nInputGroupNum * m_nOutputSize + nOutmapIndex * m_nOutputSize + i * m_nOutputWidth + j;
                    for (int k = 0; k < m_nInputGroupNum; k++)
                    {
                        nInputIndexStart = g * m_nInputGroupNum * m_nInputPadSize + k * m_nInputPadSize + (i * m_nStride) * m_nInputPadWidth + (j * m_nStride);
                        nKernelStart = g * m_nOutputGroupNum * m_nKernelSize + nOutmapIndex * m_nInputGroupNum * m_nKernelSize + k * m_nKernelSize;
                        for (int m = 0; m < m_nKernelWidth; m++)
                        {
                            for (int n = 0; n < m_nKernelWidth; n++)
                            {
                                nKernelIndex = nKernelStart + m * m_nKernelWidth + n;
                                nInputIndex = nInputIndexStart + m * m_nInputPadWidth + n;
                                fSum += m_pfInputPad[nInputIndex] * m_pfWeight[nKernelIndex];
                            }
                        }
                    }
                    if (m_pcBname != NULL)
                        fSum += m_pfBias[nOutmapIndex];

                    m_pfOutput[nOutputIndex] = fSum;
                }
            }
        }              
    }
}

void ConvLayer::ReadConvWb1()
{
	int nWsize = m_nOutputNum * m_nInputGroupNum * m_nKernelSize;
	m_pfWeight = new float[nWsize] {-8.78032e-07, -7.4436e-07, 3.63354e-07, -3.3522e-07, -5.29953e-07, 1.6597e-08, -1.17232e-06, -1.2506e-06, -1.0064e-06, -7.52674e-07, 
-3.25939e-07, 3.51299e-07, -1.11822e-07, -5.27427e-07, -3.47994e-07, 2.27993e-08, -1.27595e-06, -8.65585e-07, -6.0626e-07, -3.36897e-07, 
-3.63011e-08, -5.87131e-07, -7.0969e-07, -9.57949e-08, 8.56798e-09, -5.26638e-07, -5.11375e-07, 0.00886196, 0.0357769, -0.00778829, 
0.110731, -0.103673, -0.203748, 0.0604897, 0.0154405, -0.115601, 0.111215, -0.0168596, -0.18603, 0.154449, 
-0.28699, -0.490526, 0.111913, -0.0527142, -0.1969, 0.0368378, -0.0099768, -0.124592, 0.144294, -0.182651, 
-0.356714, 0.0358367, -0.0185475, -0.149542, -0.018843, 0.00991136, -0.00317611, 0.0261373, -0.134661, -0.162605, 
-0.0273701, 0.137786, 0.194369, 0.0313723, 0.0317805, -0.0364644, 0.0736378, -0.503994, -0.657201, 0.00912727, 
0.420397, 0.683485, -0.00836829, 0.0463911, 0.0246689, 0.0269661, -0.292484, -0.383575, -0.0174866, 0.209074, 
0.36043, 0.0659268, 0.176554, 0.217573, 0.129266, 0.270163, 0.295868, 0.165021, 0.267479, 0.270282, 
-0.0643649, -0.249915, -0.220541, -0.224293, -0.421865, -0.364307, -0.186403, -0.355824, -0.268813, 0.105343, 
0.0570823, 0.0334961, 0.101821, 0.0464219, 0.0191906, 0.0690329, 0.0654021, 0.141483, 0.0950114, -0.0142723, 
-0.0759343, -0.025078, -0.138662, -0.144093, -0.0798923, -0.163086, -0.143257, -0.0908875, -0.20354, -0.19801, 
-0.269973, -0.415537, -0.37387, -0.230755, -0.376678, -0.381107, 0.0146585, 0.275157, 0.265059, 0.244029, 
0.563378, 0.538267, 0.231919, 0.518668, 0.457109, 0.104694, -0.0163697, -0.0473475, -0.0467692, -0.197907, 
-0.00847315, -0.0521973, 0.258915, -0.0136073, 0.321299, -0.150349, -0.115065, -0.218488, -0.627973, -0.0559739, 
-0.0440349, 0.814871, 0.0915975, 0.163872, -0.0580286, -0.0829275, -0.126366, -0.33785, -0.0323095, -0.0129259, 
0.469479, 0.0346819, -0.0217146, 0.0123999, 0.106513, 0.00307743, -0.0417368, -0.101919, -0.0130511, -0.0393857, 
-0.221375, 0.0441658, 0.040443, 0.0992237, 0.0119728, -0.182083, -0.347511, -0.0379163, -0.226434, -0.575121, 
0.016818, 0.0267539, 0.120834, 0.00332573, -0.0618526, -0.175924, -0.0167067, -0.0808863, -0.354281, 0.0283103, 
0.136636, 0.103747, 0.03894, 0.110499, 0.0266268, -0.0158572, 0.0318455, -0.0901485, 0.0394129, 0.240099, 
0.179927, 0.0581161, 0.238319, 0.0890975, -0.0377793, 0.00793494, -0.205931, -0.00116467, 0.209624, 0.174268, 
0.0598846, 0.189815, 0.0679023, -0.0349465, -0.00602568, -0.190186 };
}

float *ConvLayer::GetOutput()
{
    return m_pfOutput;
}

void ConvLayer::Addpad(float *pfInput)
{
	for (int m = 0; m < m_nInputNum; m++)
	{
		for (int i = 0; i < m_nInputPadWidth; i++)
		{
			for (int j = 0; j < m_nInputPadWidth; j++)
			{
                if ((i < m_nPad) || (i >= m_nInputPadWidth - m_nPad))
                {
                    m_pfInputPad[m * m_nInputPadSize + i * m_nInputPadWidth + j] = 0;
                }
                else if ((j < m_nPad) || (j >= m_nInputPadWidth - m_nPad))
                {
                    m_pfInputPad[m * m_nInputPadSize + i * m_nInputPadWidth + j] = 0;
                }
                else
                {
                    m_pfInputPad[m * m_nInputPadSize + i * m_nInputPadWidth + j] = pfInput[m * m_nInputSize + (i - m_nPad) * m_nInputWidth + (j - m_nPad)];
                }
			}
		}
	}
}

int ConvLayer::GetOutputSize()
{
    return m_nOutputNum * m_nOutputSize;
}
