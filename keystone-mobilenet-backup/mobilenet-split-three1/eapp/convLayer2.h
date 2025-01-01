#ifndef CONVLAYER_H
#define CONVLAYER_H
#include <string>

class ConvLayer
{
public:
    //ConvLayer(const char *pcWname, int nInputNum, int nOutputNum, int nInputWidth, int nKernelWidth, int nPad, int nStride=1, int nGroup=1, const char *pcBname = NULL);
    ConvLayer(int weightName, int nInputNum, int nOutputNum, int nInputWidth, int nKernelWidth, int nPad, int nStride=1, int nGroup=1, int biasName = -1);
    ~ConvLayer();
    void forward(float *pfInput);
    float *GetOutput();
    //void ReadConvWb(const char *pcWname, const char *pcBname);
    //void ReadConvWb(int weightName, int biasName);
	void Addpad(float *pfInput);
    int GetOutputSize();
    void ReadConvWb1();
    void ReadConvWb211();
    void ReadConvWb212();
    void ReadConvWb221();
    void ReadConvWb222();
    void ReadConvWb311();
    void ReadConvWb312();
    void ReadConvWb321();
    void ReadConvWb322();
    void ReadConvWb411();
    void ReadConvWb412();
    void ReadConvWb421();
    void ReadConvWb422();
    void ReadConvWb511();
    void ReadConvWb512();
    void ReadConvWb521();
    void ReadConvWb522();
    void ReadConvWb531();
    void ReadConvWb532();
    void ReadConvWb541();
    void ReadConvWb542();
    void ReadConvWb551();
    void ReadConvWb552();
    void ReadConvWb561();
    void ReadConvWb562();
    void ReadConvWb61();
    void ReadConvWb62();
    
private:
    int m_nInputNum, m_nOutputNum, m_nInputWidth, m_nKernelWidth, m_nPad, m_nStride, m_nGroup, m_nInputGroupNum, m_nOutputGroupNum;
    float *m_pfInputPad, *m_pfOutput;
	float *m_pfWeight;
    float *m_pfBias;
    const char *m_pcBname;
    int m_nInputPadWidth, m_nOutputWidth;
    int m_nKernelSize, m_nInputSize, m_nInputPadSize, m_nOutputSize;
};

#endif
