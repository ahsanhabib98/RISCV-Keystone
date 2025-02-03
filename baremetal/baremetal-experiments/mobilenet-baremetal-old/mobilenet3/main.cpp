#include "utils.h"
#include <chrono>
#include <iostream>

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    test("data/test_list.txt",
         "data/test_labels.txt",
         "data/images/");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total execution time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
