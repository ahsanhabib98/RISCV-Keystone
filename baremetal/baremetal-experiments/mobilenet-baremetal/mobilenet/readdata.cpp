#include "readdata.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cassert>
#include <fstream>

using namespace std;

ReadData::ReadData(const char *pcMean, int nInputWidth, int nInputHeight, int nInputChannel):
    m_nInputWidth(nInputWidth), m_nInputHeight(nInputHeight), m_nInputChannel(nInputChannel)
{
    m_nImageSize = nInputWidth * nInputHeight;
    m_nInputSize = m_nImageSize * nInputChannel;
    m_pfInputData = new float[m_nInputSize];
    m_pfMean = new float[m_nInputSize];
    ReadMean(pcMean);
}

ReadData::~ReadData()
{
    delete[] m_pfInputData;
    delete[] m_pfMean;
}

float *ReadData::ReadInput(const char *pcName)
{
    cout << "Reading Picture: " << pcName << "..." << endl;

    cv::Mat srcImage = cv::imread(pcName, cv::IMREAD_UNCHANGED);
    if (srcImage.empty()) {
        cerr << "Error: Could not load image: " << pcName << endl;
        // Return m_pfInputData anyway, but note this might result in poor predictions.
        return m_pfInputData;
    }

    cv::Mat dstImage;
    cv::resize(srcImage, dstImage, cv::Size(m_nInputWidth, m_nInputHeight), 0, 0, cv::INTER_LINEAR);

    int nStep = (int)dstImage.step1();
    int nChannel = dstImage.channels();
    int nOutputIndex = 0;

    for (int i = 0; i < dstImage.rows; i++)
    {
        const uchar* pucData = dstImage.ptr<uchar>(i);
        for (int j = 0; j < dstImage.cols; j++)
        {
            nOutputIndex = i * m_nInputWidth + j;

            float bVal = (float)pucData[j * nChannel + 0];
            float gVal = (float)pucData[j * nChannel + 1];
            float rVal = (float)pucData[j * nChannel + 2];

            m_pfInputData[nOutputIndex] = bVal - m_pfMean[nOutputIndex];
            m_pfInputData[nOutputIndex + m_nImageSize] = gVal - m_pfMean[nOutputIndex + m_nImageSize];
            m_pfInputData[nOutputIndex + 2 * m_nImageSize] = rVal - m_pfMean[nOutputIndex + 2 * m_nImageSize];
        }
    }

    cout << "Reading Picture Done..." << endl;
    return m_pfInputData;
}

void ReadData::ReadMean(const char *pcMean)
{
    FILE *pM = fopen(pcMean, "rb");
    assert(pM != NULL);

    int nMsize = m_nInputSize;
    int nMreadsize = (int)fread(m_pfMean, sizeof(float), nMsize, pM);
    fclose(pM);
}
