#ifndef BATCHNORMAL2_H
#define BATCHNORMAL2_H

class BatchNormalLayer
{
public:
    BatchNormalLayer(int fileNum, int nInputNum, int nInputWidth);
    ~BatchNormalLayer();
    void forward(float *pfInput);
    float *GetOutput();
    int GetOutputSize();
    void ReadParam(int fileNum);

private:
    float m_fScale;
    int m_nInputNum, m_nInputWidth, m_nInputSize;
    float *m_pfMean, *m_pfVar, *m_pfFiller, *m_pfBias, *m_pfOutput;

};

#endif
