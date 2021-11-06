#include "CPU.h"
#include "NES.h"
#include <cstddef>
#include <cstdint>
#include <iostream>

CPU::CPU(NES* mainBus)
    : mainBus(mainBus)
{
    cycleCount = 0;

    PS.value = 0x34;
    A = X = Y = 0;
    // SP value after reset will be 0xFD
    SP = 0x00;

    // TODO: should be done by peripherals themselves
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    for (uint16_t lastBits = 0x0000; lastBits <= 0x0013; lastBits++)
    {
        uint16_t address = 0x4000 & lastBits;
        mainBus->Write(address, 0x00);
    }
    // TODO: initialize APU registers

    Reset();
}

void CPU::LoadInstrFromArray(const uint8_t* instructions, size_t number)
{
    uint8_t opcode;
    PC = 0;
    cycleCount = 0;
    while (PC < number)
    {
        Tick();
        opcode = instructions[PC++];
        ExecuteInstruction(opcode);
    }
}

CPU::CpuState CPU::GetCpuState() const
{
    return {SP, A, X, Y, PS.value, PC, cycleCount};
}

void CPU::Run() {}

void CPU::Reset()
{
    PS.Set<I>();
    Tick();
    Tick();
    Tick();
    SP--;
    Tick();
    SP--;
    Tick();
    SP--;

    uint8_t PCL = Read(0xFFFC);
    uint8_t PCH = Read(0xFFFD);
    PC = (PCH << 8) | PCL;
}

void CPU::ExecuteInstruction(uint8_t opcode)
{
    // TODO: mega switch

    Tick();
    std::cout << "Executed instruction: " << std::hex << opcode << std::endl;
}

void CPU::Tick()
{
    cycleCount++;
    mainBus->Tick();
}

uint8_t CPU::Read(uint16_t address)
{
    Tick();
    return mainBus->Read(address);
}

void CPU::Write(uint16_t address, uint8_t data)
{
    Tick();
    mainBus->Write(address, data);
}
