#include "Debugger.h"
#include <stdexcept>

Debugger::Debugger()
    : NES()
{
}

void Debugger::LoadInstrFromArray(const uint8_t* instructions, size_t number, uint16_t startingLocation)
{
    if (startingLocation + number > 0x7FF)
    {
        throw std::out_of_range("The given program would exceed RAM capacity of 2KB");
    }
    std::memcpy(RAM.data() + startingLocation, instructions, number);
}

Debugger::CpuState Debugger::ExecuteInstrFromArray(const uint8_t* instructions, size_t number,
                                                   uint16_t startingLocation)
{
    LoadInstrFromArray(instructions, number);
    cpu.ExecuteInstrFromRAM(startingLocation, number);
    return GetCpuState();
}

Debugger::CpuState Debugger::GetCpuState() const
{
    return {cpu.SP, cpu.A, cpu.X, cpu.Y, cpu.PS, cpu.PC, cpu.cycleCount};
}
