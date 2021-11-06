#include "CPU.h"
#include "NES.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sys/types.h>

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

bool CPU::PageCrossed(uint16_t address, uint16_t offset)
{
    if (((address + offset) & 0xFF00) == (address & 0xFF00))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CPU::UpdateZN(uint8_t value)
{
    if (value == 0)
    {
        PS.Set<Z>();
    }
    else
    {
        PS.Clear<Z>();
    }

    if ((value & 0x80) != 0)
    {
        PS.Set<N>();
    }
    else
    {
        PS.Clear<N>();
    }
}

uint16_t CPU::Immediate()
{
    return PC++;
}

uint16_t CPU::ZeroPage()
{
    uint8_t zeroPageAddress = Read(PC++);
    return (uint16_t)zeroPageAddress;
}

uint16_t CPU::Absolute()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    return ((uint16_t)highByte << 8) | lowByte;
}

// TODO: revisit after implementing branches
uint16_t CPU::Relative()
{
    uint8_t offset = Read(PC++);
    return offset;
}

uint16_t CPU::Indirect()
{
    uint8_t pointerLow = Read(PC++);
    uint8_t pointerHigh = Read(PC++);
    uint16_t pointer = ((uint16_t)pointerHigh << 8) | pointerLow;
    uint8_t effectiveAddressLow = Read(pointer);
    uint8_t effectiveAddressHigh = Read(pointer + 1);
    return ((uint16_t)effectiveAddressHigh << 8) | effectiveAddressLow;
}

uint16_t CPU::ZeroPageX()
{
    uint8_t zeroPageAddress = Read(PC++);
    Tick();
    return (uint16_t)(zeroPageAddress + X);
}

uint16_t CPU::ZeroPageY()
{
    uint8_t zeroPageAddress = Read(PC++);
    Tick();
    return (uint16_t)(zeroPageAddress + Y);
}

uint16_t CPU::AbsoluteX()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    uint16_t effectiveAddress = ((uint16_t)highByte << 8) | lowByte;
    if (PageCrossed(effectiveAddress, X))
    {
        Tick();
    }
    return effectiveAddress + X;
}

uint16_t CPU::AbsoluteY()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    uint16_t effectiveAddress = ((uint16_t)highByte << 8) | lowByte;
    if (PageCrossed(effectiveAddress, Y))
    {
        Tick();
    }
    return effectiveAddress + Y;
}

uint16_t CPU::IndexedIndirect()
{
    uint8_t zeroPagePointer = Read(PC++);
    Tick();
    zeroPagePointer += X;
    uint8_t effectiveAddressLow = Read(zeroPagePointer);
    uint8_t effectiveAddressHigh = Read(zeroPagePointer + 1);
    return ((uint16_t)effectiveAddressHigh << 8) | effectiveAddressLow;
}

uint16_t CPU::IndirectIndexed()
{
    uint8_t zeroPagePointer = Read(PC++);
    uint8_t effectivePointerLow = Read(zeroPagePointer);
    uint8_t effectivePointerHigh = Read(zeroPagePointer + 1);
    uint16_t effectivePointer = ((uint16_t)effectivePointerHigh << 8) | effectivePointerLow;
    if (PageCrossed(effectivePointer, Y))
    {
        Tick();
    }
    effectivePointer += Y;
    return Read(effectivePointer);
}