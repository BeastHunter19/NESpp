#include "CPU.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
    std::unique_ptr<CPU> cpu = std::make_unique<CPU>();
    std::cout << "Hello, NES!\n";
    return 0;
}
