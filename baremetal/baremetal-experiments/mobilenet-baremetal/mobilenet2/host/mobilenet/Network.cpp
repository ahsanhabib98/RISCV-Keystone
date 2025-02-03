#include "Network.h"
#include "readdata.cpp"
#include "convLayer.cpp"
#include "globalpoolLayer.cpp"
#include "batchnormalLayer.cpp"
#include "reluLayer.cpp"
#include "fcLayer.cpp"
#include "sigmoidLayer.cpp"
#include "layers_bn.cpp"
#include "layers_ds.cpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;

Network::Network()
{

	auto start2 = std::chrono::high_resolution_clock::now();

	m_Readdata = new ReadData(1, 224, 224, 3);

    m_Layers_bn = new Layers_Bn(3, 16, 224, 2, 1);

    m_Layers_ds2_1 = new Layers_Ds(16, 32, 112, 1, 211, 212);

    m_Layers_ds2_2 = new Layers_Ds(32, 64, 112, 2, 221, 222);

    m_Layers_ds3_1 = new Layers_Ds(64, 64, 56, 1, 311, 312);

    m_Layers_ds3_2 = new Layers_Ds(64, 128, 56, 2, 321, 322);

    m_Layers_ds4_1 = new Layers_Ds(128, 128, 28, 1, 411, 412);

    m_Layers_ds4_2 = new Layers_Ds(128, 256, 28, 2, 421, 422);

    m_Layers_ds5_1 = new Layers_Ds(256, 256, 14, 1, 511, 512);

    m_Layers_ds5_2 = new Layers_Ds(256, 256, 14, 1, 521, 522);

    m_Layers_ds5_3 = new Layers_Ds(256, 256, 14, 1, 531, 532);

    m_Layers_ds5_4 = new Layers_Ds(256, 256, 14, 1, 541, 542);

    m_Layers_ds5_5 = new Layers_Ds(256, 256, 14, 1, 551, 552);

    m_Layers_ds5_6 = new Layers_Ds(256, 512, 14, 2, 561, 562);

    m_Layers_ds6 = new Layers_Ds(512, 512, 7, 1, 61, 62);

    m_Poollayer6 = new GlobalPoolLayer(512, 7);

    //m_Convlayer7 = new ConvLayer("wbdata/fc7.weights", 720, 12, 1, 1, 0, 1, 1, "wbdata/fc7.biases");
    m_Fclayer7 = new FcLayer(7, 512, 12);

    m_Sigmoidlayer8 = new SigmoidLayer(12);

	
	m_vcClass.push_back("室内");
	m_vcClass.push_back("人像");
	m_vcClass.push_back("LDR");
	m_vcClass.push_back("绿植");
	m_vcClass.push_back("商场");
	m_vcClass.push_back("沙滩");
	m_vcClass.push_back("逆光");
	m_vcClass.push_back("日落");
	m_vcClass.push_back("蓝天");
	m_vcClass.push_back("雪景");
	m_vcClass.push_back("夜景");
	m_vcClass.push_back("文本");

	auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end2 - start2;

    std::cout << "Model init time: " << elapsed2.count() << " seconds" << std::endl;

}


Network::~Network()
{
	delete m_Readdata;
    delete m_Layers_bn;
    // delete m_Layers_ds1;
    delete m_Layers_ds2_1;
    delete m_Layers_ds2_2;
    delete m_Layers_ds3_1;
    delete m_Layers_ds3_2;
    delete m_Layers_ds4_1;
    delete m_Layers_ds4_2;
    delete m_Layers_ds5_1;
    delete m_Layers_ds5_2;
    delete m_Layers_ds5_3;
    delete m_Layers_ds5_4;
    delete m_Layers_ds5_5;
    delete m_Layers_ds5_6;
    delete m_Layers_ds6;
    delete m_Poollayer6;
    //delete m_Convlayer7;
    delete m_Fclayer7;
    delete m_Sigmoidlayer8;
}


float *Network::Forward()
{
    auto start3 = std::chrono::high_resolution_clock::now();

    m_Layers_bn->forward(m_Readdata->ReadInput(1));

    auto endInput = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedInput = endInput - start3;

    std::cout << "Input transmission time: " << elapsedInput.count() << " seconds" << std::endl;

    m_Layers_ds2_1->forward(m_Layers_bn->GetOutput());
    m_Layers_ds2_2->forward(m_Layers_ds2_1->GetOutput());
    
    m_Layers_ds3_1->forward(m_Layers_ds2_2->GetOutput());
    m_Layers_ds3_2->forward(m_Layers_ds3_1->GetOutput());

    m_Layers_ds4_1->forward(m_Layers_ds3_2->GetOutput());
    m_Layers_ds4_2->forward(m_Layers_ds4_1->GetOutput());

    m_Layers_ds5_1->forward(m_Layers_ds4_2->GetOutput());
    m_Layers_ds5_2->forward(m_Layers_ds5_1->GetOutput());
    m_Layers_ds5_3->forward(m_Layers_ds5_2->GetOutput());
    m_Layers_ds5_4->forward(m_Layers_ds5_3->GetOutput());
    m_Layers_ds5_5->forward(m_Layers_ds5_4->GetOutput());
    m_Layers_ds5_6->forward(m_Layers_ds5_5->GetOutput());

    m_Layers_ds6->forward(m_Layers_ds5_6->GetOutput());

    m_Poollayer6->forward(m_Layers_ds6->GetOutput());

    //m_Convlayer7->forward(m_Poollayer6->GetOutput());
    m_Fclayer7->forward(m_Poollayer6->GetOutput());

    //m_Sigmoidlayer8->forward(m_Convlayer7->GetOutput());
    m_Sigmoidlayer8->forward(m_Fclayer7->GetOutput());

    float *pfOutput = m_Sigmoidlayer8->GetOutput();
	vector <int> argmax;
	vector <float> Max;

    //int nOutputSize = m_Convlayer7->GetOutputSize();
    int nOutputSize = m_Fclayer7->GetOutputSize();
	for (int i = 0; i<nOutputSize; i++)
	{

		argmax.push_back(i);
		Max.push_back(pfOutput[i]);
	}
	cout << endl;
	for (int i = 0; i < argmax.size(); i++)
	{
		cout << Max[i] << ": " << argmax[i] << ": " << m_vcClass[argmax[i]] << endl;
	}
	cout << endl;

    auto end3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed3 = end3 - start3;

    std::cout << "Model inference time: " << elapsed3.count() << " seconds" << std::endl;
    
    return pfOutput;
}
