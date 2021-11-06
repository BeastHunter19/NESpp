#include "NES.h"

NES::NES()
    : cpu(this)
{
    ResetRAM();
}

uint8_t NES::Read(uint16_t address) const
{
    return uint8_t{};
}

void NES::Write(uint16_t address, uint8_t data) {}

void NES::Tick()
{
    // TODO: step all the peripherals
}

void NES::ResetRAM()
{
    RAM.fill(0xFF);
}

CPU::CpuState NES::ExecuteInstrFromArray(const uint8_t* instructions, size_t number)
{
    cpu.LoadInstrFromArray(instructions, number);
    return cpu.GetCpuState();
}
