#ifndef UTILS_H
#define UTILS_H

#include <string>

float Accuracy_1(float *pfPred, int *pnLab, int nclass);
float Accuracy_all(float *pfPred, int *pnLab, int nclass);
//void test(const char *pcTestListName, const char *pcTestLabelsName, const char *pcTestImageSet, const std::string& filepath);
void test();

#endif
