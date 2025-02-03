#ifndef FCLAYER_H
#define FCLAYER_H
#include <string>

class FcLayer
{
public:
    //FcLayer(const char *pcWname, const char *pcBname, int nInputSize, int nOutputSize, int nRelubool);
    FcLayer(int fileNum, int nInputSize, int nOutputSize);
    FcLayer(const char *pcWname, const char *pcBname, int nInputSize, int nOutputSize, int nRelubool);
    ~FcLayer();
    void forward(float *pfInput);
    float *GetOutput();
    int GetOutputSize();
    //void ReadFcWb(const char *pcWname, const char *pcBname);
    void ReadFcWb(int fileNum);
    
private:
    int m_nInputSize, m_nOutputSize, m_nWeightSize, m_nRelubool;
      const char* m_pcWname;
    const char* m_pcBname;
    float *m_pfWeight, *m_pfBias, *m_pfOutput;
};

#endif