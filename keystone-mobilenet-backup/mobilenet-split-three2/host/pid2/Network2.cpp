#include "Network2.h"
#include "batchnormalLayer2.cpp"
#include "convLayer2.cpp"
#include "fcLayer.cpp"
#include "globalpoolLayer.cpp"
#include "layers_bn2.cpp"
#include "layers_ds.cpp"
#include "reluLayer.cpp"
#include "sigmoidLayer.cpp"
#include <vector>
#include <iostream>
#include "string.h"

#include "eapp_utils.h"
#include "edge_call.h"
#include "syscall.h"

#include <chrono>
#include <iomanip>

#include "crypto.h"
#include "aes.hpp"

using namespace std;

Network::Network()
{
	print_time2("Network Init 2 Start");
	
	cout << "Initializing Network 2...\n" << endl;

	// Second Layer
	m_Layers_ds2_1 = new Layers_Ds(16, 32, 112, 1, 211, 212);  // Reduced from 32 -> 64 to 22 -> 45
	m_Layers_ds2_2 = new Layers_Ds(32, 64, 112, 2, 221, 222);  // Reduced from 64 -> 128 to 45 -> 90
	
	// Third Layer
	m_Layers_ds3_1 = new Layers_Ds(64, 64, 56, 1, 311, 312);  // Reduced from 128 -> 128 to 90 -> 90
	m_Layers_ds3_2 = new Layers_Ds(64, 128, 56, 2, 321, 322);  // Reduced from 128 -> 256 to 90 -> 180

	// Fourth Layer
	m_Layers_ds4_1 = new Layers_Ds(128, 128, 28, 1, 411, 412);  // Reduced from 256 -> 256 to 180 -> 180
	m_Layers_ds4_2 = new Layers_Ds(128, 256, 28, 2, 421, 422);  // Reduced from 256 -> 512 to 180 -> 360
	
	// Fifth Layer
	m_Layers_ds5_1 = new Layers_Ds(256, 256, 14, 1, 511, 512);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_2 = new Layers_Ds(256, 256, 14, 1, 521, 522);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_3 = new Layers_Ds(256, 256, 14, 1, 531, 532);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_4 = new Layers_Ds(256, 256, 14, 1, 541, 542);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_5 = new Layers_Ds(256, 256, 14, 1, 551, 552);  // Reduced from 512 -> 512 to 360 -> 360
	m_Layers_ds5_6 = new Layers_Ds(256, 512, 14, 2, 561, 562);  // Reduced from 512 -> 1024 to 360 -> 720
	
	// Sixth Layer
	m_Layers_ds6 = new Layers_Ds(512, 512, 7, 1, 61, 62);  // Reduced from 1024 -> 1024 to 720 -> 720

	// Global Pooling Layer
	m_Poollayer6 = new GlobalPoolLayer(512, 7);  // Reduced from 1024 to 720
	
	cout << "Initializing Network 2 Done...\n" << endl;
	
	print_time2("Network Init 2 End");
}


Network::~Network()
{
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
}

float* Network::Forward(float* input)
{
	print_time2("Inference 2 Start");

	m_Layers_ds2_1->forward(input);
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
	m_Layers_ds5_6->forward(m_Layers_ds5_5->GetOutput());
	
	m_Layers_ds6->forward(m_Layers_ds5_6->GetOutput());

	m_Poollayer6->forward(m_Layers_ds6->GetOutput());
	
	m_pfOutput = m_Poollayer6->GetOutput();
	
	print_time2("Inference 2 End");
	
	return m_pfOutput;
}

unsigned long print_time2(char* str)
{
	// Get the current time point from the system clock
  auto now = std::chrono::system_clock::now();
  
  // Convert the time point to a time_t which represents the time in seconds
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  
  // Convert the time_t to a tm struct for formatting
  std::tm* now_tm = std::localtime(&now_time_t);
  
  // Get the number of milliseconds since the last second
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // Print the time with millisecond precision
  std::cout << "Time: ";
  std::cout << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
  std::cout << '.' << std::setfill('0') << std::setw(3) << milliseconds.count() << " : ";
	return printf("%s\n", str); 
}
