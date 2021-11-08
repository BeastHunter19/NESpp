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
    for (size_t instr = 0; instr < number; instr++)
    {
        Write(0x0700 + instr, instructions[instr]);
    }

    cycleCount = 0;
    PC = 0x0700;
    while (PC < (0x0700 + number) && cycleCount < CYCLES_PER_FRAME)
    {
        opcode = Read(PC++);
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
    switch (opcode)
    {
    case 0x00: Illegal(); break;
    case 0x01: Illegal(); break;
    case 0x02: Illegal(); break;
    case 0x03: Illegal(); break;
    case 0x04: Illegal(); break;
    case 0x05: Illegal(); break;
    case 0x06: Illegal(); break;
    case 0x07: Illegal(); break;
    case 0x08: Illegal(); break;
    case 0x09: Illegal(); break;
    case 0x0A: Illegal(); break;
    case 0x0B: Illegal(); break;
    case 0x0C: Illegal(); break;
    case 0x0D: Illegal(); break;
    case 0x0E: Illegal(); break;
    case 0x0F: Illegal(); break;
    case 0x10: Illegal(); break;
    case 0x11: Illegal(); break;
    case 0x12: Illegal(); break;
    case 0x13: Illegal(); break;
    case 0x14: Illegal(); break;
    case 0x15: Illegal(); break;
    case 0x16: Illegal(); break;
    case 0x17: Illegal(); break;
    case 0x18: CLC<&CPU::Implied>(); break;
    case 0x19: Illegal(); break;
    case 0x1A: Illegal(); break;
    case 0x1B: Illegal(); break;
    case 0x1C: Illegal(); break;
    case 0x1D: Illegal(); break;
    case 0x1E: Illegal(); break;
    case 0x1F: Illegal(); break;
    case 0x20: Illegal(); break;
    case 0x21: Illegal(); break;
    case 0x22: Illegal(); break;
    case 0x23: Illegal(); break;
    case 0x24: Illegal(); break;
    case 0x25: Illegal(); break;
    case 0x26: Illegal(); break;
    case 0x27: Illegal(); break;
    case 0x28: Illegal(); break;
    case 0x29: Illegal(); break;
    case 0x2A: Illegal(); break;
    case 0x2B: Illegal(); break;
    case 0x2C: Illegal(); break;
    case 0x2D: Illegal(); break;
    case 0x2E: Illegal(); break;
    case 0x2F: Illegal(); break;
    case 0x30: Illegal(); break;
    case 0x31: Illegal(); break;
    case 0x32: Illegal(); break;
    case 0x33: Illegal(); break;
    case 0x34: Illegal(); break;
    case 0x35: Illegal(); break;
    case 0x36: Illegal(); break;
    case 0x37: Illegal(); break;
    case 0x38: SEC<&CPU::Implied>(); break;
    case 0x39: Illegal(); break;
    case 0x3A: Illegal(); break;
    case 0x3B: Illegal(); break;
    case 0x3C: Illegal(); break;
    case 0x3D: Illegal(); break;
    case 0x3E: Illegal(); break;
    case 0x3F: Illegal(); break;
    case 0x40: Illegal(); break;
    case 0x41: Illegal(); break;
    case 0x42: Illegal(); break;
    case 0x43: Illegal(); break;
    case 0x44: Illegal(); break;
    case 0x45: Illegal(); break;
    case 0x46: Illegal(); break;
    case 0x47: Illegal(); break;
    case 0x48: Illegal(); break;
    case 0x49: Illegal(); break;
    case 0x4A: Illegal(); break;
    case 0x4B: Illegal(); break;
    case 0x4C: JMP<&CPU::Absolute>(); break;
    case 0x4D: Illegal(); break;
    case 0x4E: Illegal(); break;
    case 0x4F: Illegal(); break;
    case 0x50: Illegal(); break;
    case 0x51: Illegal(); break;
    case 0x52: Illegal(); break;
    case 0x53: Illegal(); break;
    case 0x54: Illegal(); break;
    case 0x55: Illegal(); break;
    case 0x56: Illegal(); break;
    case 0x57: Illegal(); break;
    case 0x58: Illegal(); break;
    case 0x59: Illegal(); break;
    case 0x5A: Illegal(); break;
    case 0x5B: Illegal(); break;
    case 0x5C: Illegal(); break;
    case 0x5D: Illegal(); break;
    case 0x5E: Illegal(); break;
    case 0x5F: Illegal(); break;
    case 0x60: Illegal(); break;
    case 0x61: ADC<&CPU::IndexedIndirect>(); break;
    case 0x62: Illegal(); break;
    case 0x63: Illegal(); break;
    case 0x64: Illegal(); break;
    case 0x65: ADC<&CPU::ZeroPage>(); break;
    case 0x66: Illegal(); break;
    case 0x67: Illegal(); break;
    case 0x68: Illegal(); break;
    case 0x69: ADC<&CPU::Immediate>(); break;
    case 0x6A: Illegal(); break;
    case 0x6B: Illegal(); break;
    case 0x6C: JMP<&CPU::Indirect>(); break;
    case 0x6D: ADC<&CPU::Absolute>(); break;
    case 0x6E: Illegal(); break;
    case 0x6F: Illegal(); break;
    case 0x70: Illegal(); break;
    case 0x71: ADC<&CPU::IndirectIndexed>(); break;
    case 0x72: Illegal(); break;
    case 0x73: Illegal(); break;
    case 0x74: Illegal(); break;
    case 0x75: ADC<&CPU::ZeroPageX>(); break;
    case 0x76: Illegal(); break;
    case 0x77: Illegal(); break;
    case 0x78: Illegal(); break;
    case 0x79: ADC<&CPU::AbsoluteY>(); break;
    case 0x7A: Illegal(); break;
    case 0x7B: Illegal(); break;
    case 0x7C: Illegal(); break;
    case 0x7D: ADC<&CPU::AbsoluteX>(); break;
    case 0x7E: Illegal(); break;
    case 0x7F: Illegal(); break;
    case 0x80: Illegal(); break;
    case 0x81: STA<&CPU::IndexedIndirect>(); break;
    case 0x82: Illegal(); break;
    case 0x83: Illegal(); break;
    case 0x84: Illegal(); break;
    case 0x85: STA<&CPU::ZeroPage>(); break;
    case 0x86: Illegal(); break;
    case 0x87: Illegal(); break;
    case 0x88: Illegal(); break;
    case 0x89: Illegal(); break;
    case 0x8A: Illegal(); break;
    case 0x8B: Illegal(); break;
    case 0x8C: Illegal(); break;
    case 0x8D: STA<&CPU::Absolute>(); break;
    case 0x8E: Illegal(); break;
    case 0x8F: Illegal(); break;
    case 0x90: Illegal(); break;
    case 0x91: STA<&CPU::IndirectIndexed>(); break;
    case 0x92: Illegal(); break;
    case 0x93: Illegal(); break;
    case 0x94: Illegal(); break;
    case 0x95: STA<&CPU::ZeroPageX>(); break;
    case 0x96: Illegal(); break;
    case 0x97: Illegal(); break;
    case 0x98: Illegal(); break;
    case 0x99: STA<&CPU::AbsoluteY>(); break;
    case 0x9A: Illegal(); break;
    case 0x9B: Illegal(); break;
    case 0x9C: Illegal(); break;
    case 0x9D: STA<&CPU::AbsoluteX>(); break;
    case 0x9E: Illegal(); break;
    case 0x9F: Illegal(); break;
    case 0xA0: LDY<&CPU::Immediate>(); break;
    case 0xA1: LDA<&CPU::IndexedIndirect>(); break;
    case 0xA2: LDX<&CPU::Immediate>(); break;
    case 0xA3: Illegal(); break;
    case 0xA4: LDY<&CPU::ZeroPage>(); break;
    case 0xA5: LDA<&CPU::ZeroPage>(); break;
    case 0xA6: LDX<&CPU::ZeroPage>(); break;
    case 0xA7: Illegal(); break;
    case 0xA8: Illegal(); break;
    case 0xA9: LDA<&CPU::Immediate>(); break;
    case 0xAA: Illegal(); break;
    case 0xAB: Illegal(); break;
    case 0xAC: LDY<&CPU::Absolute>(); break;
    case 0xAD: LDA<&CPU::Absolute>(); break;
    case 0xAE: LDX<&CPU::Absolute>(); break;
    case 0xAF: Illegal(); break;
    case 0xB0: Illegal(); break;
    case 0xB1: LDA<&CPU::IndirectIndexed>(); break;
    case 0xB2: Illegal(); break;
    case 0xB3: Illegal(); break;
    case 0xB4: LDY<&CPU::ZeroPageX>(); break;
    case 0xB5: LDA<&CPU::ZeroPageX>(); break;
    case 0xB6: LDX<&CPU::ZeroPageY>(); break;
    case 0xB7: Illegal(); break;
    case 0xB8: Illegal(); break;
    case 0xB9: LDA<&CPU::AbsoluteY>(); break;
    case 0xBA: Illegal(); break;
    case 0xBB: Illegal(); break;
    case 0xBC: LDY<&CPU::AbsoluteX>(); break;
    case 0xBD: LDA<&CPU::AbsoluteX>(); break;
    case 0xBE: LDX<&CPU::AbsoluteY>(); break;
    case 0xBF: Illegal(); break;
    case 0xC0: Illegal(); break;
    case 0xC1: Illegal(); break;
    case 0xC2: Illegal(); break;
    case 0xC3: Illegal(); break;
    case 0xC4: Illegal(); break;
    case 0xC5: Illegal(); break;
    case 0xC6: Illegal(); break;
    case 0xC7: Illegal(); break;
    case 0xC8: Illegal(); break;
    case 0xC9: Illegal(); break;
    case 0xCA: Illegal(); break;
    case 0xCB: Illegal(); break;
    case 0xCC: Illegal(); break;
    case 0xCD: Illegal(); break;
    case 0xCE: Illegal(); break;
    case 0xCF: Illegal(); break;
    case 0xD0: Illegal(); break;
    case 0xD1: Illegal(); break;
    case 0xD2: Illegal(); break;
    case 0xD3: Illegal(); break;
    case 0xD4: Illegal(); break;
    case 0xD5: Illegal(); break;
    case 0xD6: Illegal(); break;
    case 0xD7: Illegal(); break;
    case 0xD8: Illegal(); break;
    case 0xD9: Illegal(); break;
    case 0xDA: Illegal(); break;
    case 0xDB: Illegal(); break;
    case 0xDC: Illegal(); break;
    case 0xDD: Illegal(); break;
    case 0xDE: Illegal(); break;
    case 0xDF: Illegal(); break;
    case 0xE0: Illegal(); break;
    case 0xE1: Illegal(); break;
    case 0xE2: Illegal(); break;
    case 0xE3: Illegal(); break;
    case 0xE4: Illegal(); break;
    case 0xE5: Illegal(); break;
    case 0xE6: Illegal(); break;
    case 0xE7: Illegal(); break;
    case 0xE8: Illegal(); break;
    case 0xE9: Illegal(); break;
    case 0xEA: NOP<&CPU::Implied>(); break;
    case 0xEB: Illegal(); break;
    case 0xEC: Illegal(); break;
    case 0xED: Illegal(); break;
    case 0xEE: Illegal(); break;
    case 0xEF: Illegal(); break;
    case 0xF0: Illegal(); break;
    case 0xF1: Illegal(); break;
    case 0xF2: Illegal(); break;
    case 0xF3: Illegal(); break;
    case 0xF4: Illegal(); break;
    case 0xF5: Illegal(); break;
    case 0xF6: Illegal(); break;
    case 0xF7: Illegal(); break;
    case 0xF8: Illegal(); break;
    case 0xF9: Illegal(); break;
    case 0xFA: Illegal(); break;
    case 0xFB: Illegal(); break;
    case 0xFC: Illegal(); break;
    case 0xFD: Illegal(); break;
    case 0xFE: Illegal(); break;
    case 0xFF: Illegal(); break;
    }
    // std::cout << "Executed instruction: " << std::hex << opcode << std::endl;
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

// ADDDRESSING MODES

int CPU::Implied()
{
    return 0;
}

int CPU::Immediate()
{
    address = PC++;
    return 0;
}

int CPU::ZeroPage()
{
    uint8_t zeroPageAddress = Read(PC++);
    address = (uint16_t)zeroPageAddress;
    return 0;
}

int CPU::Absolute()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    address = ((uint16_t)highByte << 8) | lowByte;
    return 0;
}

// TODO: revisit after implementing branches
int CPU::Relative()
{
    uint8_t offset = Read(PC++);
    return offset;
}

int CPU::Indirect()
{
    uint8_t pointerLow = Read(PC++);
    uint8_t pointerHigh = Read(PC++);
    uint16_t pointer = ((uint16_t)pointerHigh << 8) | pointerLow;
    uint8_t effectiveAddressLow = Read(pointer);
    // If the low byte is at a page boundary, the high one is
    // fetched from the start of the same page due to a bug
    // in the original 6502
    pointerLow++;
    pointer = ((uint16_t)pointerHigh << 8) | pointerLow;
    uint8_t effectiveAddressHigh = Read(pointer);
    address = ((uint16_t)effectiveAddressHigh << 8) | effectiveAddressLow;
    return 0;
}

int CPU::ZeroPageX()
{
    uint8_t zeroPageAddress = Read(PC++);
    Tick();
    address = (uint16_t)((zeroPageAddress + X) % 256);
    return 0;
}

int CPU::ZeroPageY()
{
    uint8_t zeroPageAddress = Read(PC++);
    Tick();
    address = (uint16_t)((zeroPageAddress + Y) % 256);
    return 0;
}

int CPU::AbsoluteX()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    uint16_t effectiveAddress = ((uint16_t)highByte << 8) | lowByte;
    address = effectiveAddress + X;
    if (PageCrossed(effectiveAddress, X))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int CPU::AbsoluteY()
{
    uint8_t lowByte = Read(PC++);
    uint8_t highByte = Read(PC++);
    uint16_t effectiveAddress = ((uint16_t)highByte << 8) | lowByte;
    address = effectiveAddress + Y;
    if (PageCrossed(effectiveAddress, Y))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int CPU::IndexedIndirect()
{
    uint8_t zeroPagePointer = Read(PC++);
    Tick();
    zeroPagePointer += X;
    uint8_t effectiveAddressLow = Read(zeroPagePointer);
    uint8_t effectiveAddressHigh = Read(zeroPagePointer + 1);
    address = ((uint16_t)effectiveAddressHigh << 8) | effectiveAddressLow;
    return 0;
}

int CPU::IndirectIndexed()
{
    uint8_t zeroPagePointer = Read(PC++);
    uint8_t effectivePointerLow = Read(zeroPagePointer++);
    uint8_t effectivePointerHigh = Read(zeroPagePointer);
    uint16_t effectivePointer = ((uint16_t)effectivePointerHigh << 8) | effectivePointerLow;
    address = effectivePointer + Y;
    if (PageCrossed(effectivePointer, Y))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// INSTRUCTIONS

void CPU::Illegal()
{
    Tick();
}

template <CPU::AddressModePtr AddrMode>
void CPU::ADC()
{
    (this->*AddrMode)();
    uint8_t operand = Read(address);
    uint8_t carry = PS.Test<C>();
    uint8_t result = A + operand + carry;
    UpdateZN(result);
    if (result < A)
    {
        PS.Set<C>();
    }
    else
    {
        PS.Clear<C>();
    }

    // Overflow is set if the two operands have the same sign
    // (both 0 or both 1) and the resul have a different one
    if ((~(A ^ (operand + carry)) & (A ^ result)) & 0x80)
    {
        PS.Set<V>();
    }
    else
    {
        PS.Clear<V>();
    }
    A = result;
}

template <CPU::AddressModePtr AddrMode>
void CPU::CLC()
{
    Tick();
    PS.Clear<C>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::JMP()
{
    (this->*AddrMode)();
    PC = address;
}

template <CPU::AddressModePtr AddrMode>
void CPU::LDA()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    UpdateZN(operand);
    A = operand;
}

template <CPU::AddressModePtr AddrMode>
void CPU::LDX()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    UpdateZN(operand);
    X = operand;
}

template <CPU::AddressModePtr AddrMode>
void CPU::LDY()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    UpdateZN(operand);
    Y = operand;
}

template <CPU::AddressModePtr AddrMode>
void CPU::NOP()
{
    Tick();
}

template <CPU::AddressModePtr AddrMode>
void CPU::SEC()
{
    Tick();
    PS.Set<C>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::STA()
{
    (this->*AddrMode)();
    if (AddrMode == &CPU::AbsoluteX || AddrMode == &CPU::AbsoluteY || AddrMode == &CPU::IndirectIndexed)
    {
        Tick();
    }
    Write(address, A);
}