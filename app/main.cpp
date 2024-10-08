#include "NESpp/Debugger.h"
#include "NESpp/Emulator.h"
#include <chrono>
#include <iostream>
#include <memory>

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
    /*
    Emulator nes;
    Debugger debugger(nes);
    uint8_t testRom[]{0xA9, 0x00, 0xA2, 0x1F, 0x9D, 0x00, 0x03, 0xBD, 0x00,
                      0x03, 0x69, 0x01, 0x9D, 0x00, 0x03, 0x4C, 0x07, 0x07};
    debugger.ExecuteInstrFromArray(testRom, 18);

    size_t number;
    std::string disassembled[256];
    {
        Timer t;
        number = debugger.Disassembly(disassembled, 0x0700, 18);
    }
    for (size_t i = 0; i < number; i++)
    {
        std::cout << disassembled[i] << '\n';
    }
    */

    // Loading ROM passed as command line argument
    if(argc < 2) return -1;

    Emulator mainEmulator;
    Debugger mainDebugger(mainEmulator);
    mainDebugger.LoadROM(argv[1]);
    mainDebugger.SetPC(0xC000);
    mainDebugger.RunWithTrace();
    uint8_t byte1 = mainDebugger.GetMemoryState().at(0x00);
    uint8_t byte2 = mainDebugger.GetMemoryState().at(0x01);
    Debugger::CpuState state = mainDebugger.GetCpuState();

    uint8_t test1, test2, test3;
    test1 = mainDebugger.GetPRG_ROM().at(0x0FF2);
    test2 = mainDebugger.GetPRG_ROM().at(0x0FF3);
    test3 = mainDebugger.GetPRG_ROM().at(0x0FF4);

    std::cout << "byte 0x00: " << std::hex << (int)byte1 << ", byte 0x01: " << std::hex << (int)byte2 << std::endl;
    std::cout << "A:" << std::hex << (int)state.A << ", X:" << (int)state.X << ", Y:" << (int)state.Y << ", PS:" << (int)state.PS.value << ", SP:" << (int)state.SP << ", cycles:" << state.cycleCount << std::endl;
    std::cout << "CFF2: " << std::hex << (int)test1 << "  CFF3: " << (int)test2 << "  CFF4: " << (int)test3 << std::endl;
    return 0;
}
