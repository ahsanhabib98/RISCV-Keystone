#include "layers_bn.h"

Layers_Bn::Layers_Bn(int nInputNum, int nOutputNum, int nInputWidth, int nStride, int fileNum)
{
    m_ConvlayerDw = new ConvLayer(fileNum, nInputNum, nOutputNum, nInputWidth, 3, 1, nStride);
    m_ConvDwBN = new BatchNormalLayer(fileNum, nOutputNum, nInputWidth / nStride);
    m_RelulayerDw = new ReluLayer(m_ConvDwBN->GetOutputSize());
}

Layers_Bn::~Layers_Bn()
{
    delete m_ConvlayerDw;
    delete m_ConvDwBN;
    delete m_RelulayerDw;
}

void Layers_Bn::forward(float *pfInput)
{
    m_ConvlayerDw->forward(pfInput);
    m_ConvDwBN->forward(m_ConvlayerDw->GetOutput());
    m_RelulayerDw->forward(m_ConvDwBN->GetOutput());
}

float *Layers_Bn::GetOutput()
{
    return m_RelulayerDw->GetOutput();
}