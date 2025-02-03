#ifndef READDATA_H
#define READDATA_H


class ReadData
{
public:
	ReadData(int fileNum, int nInputWidth, int nInputHeight, int nInputChannel);
	~ReadData();
	float *ReadInput(int imgNum);
	void ReadMean(int fileNum);

private:
	int m_nInputSize, m_nInputWidth, m_nInputHeight, m_nInputChannel, m_nImageSize;
	float *m_pfInputData, *m_pfMean;

};


#endif