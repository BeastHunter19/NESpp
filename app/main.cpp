#include "NESpp/Debugger.h"
#include <iostream>
#include <memory>

#include <chrono>

struct Timer
{
    std::chrono::system_clock::time_point start, end;
    std::chrono::duration<float> duration;

    Timer() { start = std::chrono::high_resolution_clock::now(); }

    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        float ms = duration.count() * 1000.0f;
        std::cerr << "Timer took " << ms << " ms\n";
    }
};

int main(int argc, char** argv)
{
    std::cout << "Hello, NES!\n\n";

    Debugger emulator;
    uint8_t testRom[]{0xA9, 0x00, 0xA2, 0x1F, 0x9D, 0x00, 0x03, 0xBD, 0x00,
                      0x03, 0x69, 0x01, 0x9D, 0x00, 0x03, 0x4C, 0x07, 0x07};
    Timer t;
    emulator.ExecuteInstrFromArray(testRom, 18);

    return 0;
}
