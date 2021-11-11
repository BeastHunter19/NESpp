#include "Debugger.h"
#include <stdexcept>

void Debugger::LoadInstrFromArray(const uint8_t* instructions, size_t number, uint16_t startingLocation)
{
    if (startingLocation + number > 0x7FF)
    {
        throw std::out_of_range("The given program would exceed RAM capacity of 2KB");
    }
    std::memcpy(core->RAM.data() + startingLocation, instructions, number);
}

Debugger::CpuState Debugger::ExecuteInstrFromArray(const uint8_t* instructions, size_t number,
                                                   uint16_t startingLocation)
{
    LoadInstrFromArray(instructions, number);
    core->cpu.ExecuteInstrFromRAM(startingLocation, number);
    return GetCpuState();
}

Debugger::CpuState Debugger::GetCpuState() const
{
    return {core->cpu.SP, core->cpu.A, core->cpu.X, core->cpu.Y, core->cpu.PS, core->cpu.PC, core->cpu.cycleCount};
}
