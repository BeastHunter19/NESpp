#include "CPU.h"
#include "MainBus.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sys/types.h>

CPU::CPU(MainBus& mainBus)
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
        mainBus.Write(address, 0x00);
    }
    // TODO: initialize APU registers

    Reset();

    opcodeTable = {
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x00
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x01
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x02
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x03
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x04
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x05
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x06
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x07
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x08
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x09
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x0F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x10
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x11
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x12
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x13
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x14
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x15
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x16
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x17
        {&CPU::CLC<&CPU::Implied>, "CLC", 1, 2},         // 0x18
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x19
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x1F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x20
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x21
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x22
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x23
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x24
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x25
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x26
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x27
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x28
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x29
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x2F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x30
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x31
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x32
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x33
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x34
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x35
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x36
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x37
        {&CPU::SEC<&CPU::Implied>, "SEC", 1, 2},         // 0x38
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x39
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x3F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x40
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x41
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x42
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x43
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x44
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x45
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x46
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x47
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x48
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x49
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x4A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x4B
        {&CPU::JMP<&CPU::Absolute>, "JMP", 3, 3},        // 0x4C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x4D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x4E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x4F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x50
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x51
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x52
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x53
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x54
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x55
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x56
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x57
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x58
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x59
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5C
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x5F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x60
        {&CPU::ADC<&CPU::IndexedIndirect>, "ADC", 2, 6}, // 0x61
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x62
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x63
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x64
        {&CPU::ADC<&CPU::ZeroPage>, "ADC", 2, 3},        // 0x65
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x66
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x67
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x68
        {&CPU::ADC<&CPU::Immediate>, "ADC", 2, 2},       // 0x69
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x6A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x6B
        {&CPU::JMP<&CPU::Indirect>, "JMP", 3, 5},        // 0x6C
        {&CPU::ADC<&CPU::Absolute>, "ADC", 3, 4},        // 0x6D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x6E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x6F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x70
        {&CPU::ADC<&CPU::IndirectIndexed>, "ADC", 2, 5}, // 0x71
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x72
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x73
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x74
        {&CPU::ADC<&CPU::ZeroPageX>, "ADC", 2, 4},       // 0x75
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x76
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x77
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x78
        {&CPU::ADC<&CPU::AbsoluteY>, "ADC", 3, 4},       // 0x79
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x7A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x7B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x7C
        {&CPU::ADC<&CPU::AbsoluteX>, "ADC", 3, 4},       // 0x7D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x7E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x7F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x80
        {&CPU::STA<&CPU::IndexedIndirect>, "STA", 2, 6}, // 0x81
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x82
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x83
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x84
        {&CPU::STA<&CPU::ZeroPage>, "STA", 2, 3},        // 0x85
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x86
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x87
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x88
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x89
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x8A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x8B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x8C
        {&CPU::STA<&CPU::Absolute>, "STA", 3, 4},        // 0x8D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x8E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x8F
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x90
        {&CPU::STA<&CPU::IndirectIndexed>, "STA", 2, 6}, // 0x91
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x92
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x93
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x94
        {&CPU::STA<&CPU::ZeroPageX>, "STA", 2, 4},       // 0x95
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x96
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x97
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x98
        {&CPU::STA<&CPU::AbsoluteY>, "STA", 3, 5},       // 0x99
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x9A
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x9B
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x9C
        {&CPU::STA<&CPU::AbsoluteX>, "STA", 3, 5},       // 0x9D
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x9E
        {&CPU::Illegal, "ILL", 1, 2},                    // 0x9F
        {&CPU::LDY<&CPU::Immediate>, "LDY", 2, 2},       // 0xA0
        {&CPU::LDA<&CPU::IndexedIndirect>, "LDA", 2, 6}, // 0xA1
        {&CPU::LDX<&CPU::Immediate>, "LDX", 2, 2},       // 0xA2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xA3
        {&CPU::LDY<&CPU::ZeroPage>, "LDY", 2, 3},        // 0xA4
        {&CPU::LDA<&CPU::ZeroPage>, "LDA", 2, 3},        // 0xA5
        {&CPU::LDX<&CPU::ZeroPage>, "LDX", 2, 3},        // 0xA6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xA7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xA8
        {&CPU::LDA<&CPU::Immediate>, "LDA", 2, 2},       // 0xA9
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xAA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xAB
        {&CPU::LDY<&CPU::Absolute>, "LDY", 3, 4},        // 0xAC
        {&CPU::LDA<&CPU::Absolute>, "LDA", 3, 4},        // 0xAD
        {&CPU::LDX<&CPU::Absolute>, "LDX", 3, 4},        // 0xAE
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xAF
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xB0
        {&CPU::LDA<&CPU::IndirectIndexed>, "LDA", 2, 5}, // 0xB1
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xB2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xB3
        {&CPU::LDY<&CPU::ZeroPageX>, "LDY", 2, 4},       // 0xB4
        {&CPU::LDA<&CPU::ZeroPageX>, "LDA", 2, 4},       // 0xB5
        {&CPU::LDX<&CPU::ZeroPageY>, "LDX", 2, 4},       // 0xB6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xB7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xB8
        {&CPU::LDA<&CPU::AbsoluteY>, "LDA", 3, 4},       // 0xB9
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xBA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xBB
        {&CPU::LDY<&CPU::AbsoluteX>, "LDY", 3, 4},       // 0xBC
        {&CPU::LDA<&CPU::AbsoluteX>, "LDA", 3, 4},       // 0xBD
        {&CPU::LDX<&CPU::AbsoluteY>, "LDX", 3, 4},       // 0xBE
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xBF
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC0
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC1
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC3
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC4
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC5
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC8
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xC9
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCB
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCC
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCD
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCE
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xCF
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD0
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD1
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD3
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD4
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD5
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD8
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xD9
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDB
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDC
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDD
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDE
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xDF
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE0
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE1
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE3
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE4
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE5
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE8
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xE9
        {&CPU::NOP<&CPU::Implied>, "NOP", 1, 2},         // 0xEA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xEB
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xEC
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xED
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xEE
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xEF
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF0
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF1
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF2
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF3
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF4
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF5
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF6
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF7
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF8
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xF9
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xFA
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xFB
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xFC
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xFD
        {&CPU::Illegal, "ILL", 1, 2},                    // 0xFE
    };
}

void CPU::ExecuteInstrFromRAM(uint16_t startingLocation, size_t number)
{
    cycleCount = 0;
    PC = startingLocation;
    while (PC < (startingLocation + number) && cycleCount < CYCLES_PER_FRAME)
    {
        opcode = Read(PC++);
        ExecuteInstruction();
    }
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

void CPU::ExecuteInstruction()
{
    (this->*(opcodeTable[opcode].ptr))();
    // std::cout << "Executed instruction: " << opcodeTable[opcode].mnemonic << '\n';
}

void CPU::Tick()
{
    cycleCount++;
    mainBus.Tick();
}

uint8_t CPU::Read(uint16_t address)
{
    Tick();
    return mainBus.Read(address);
}

void CPU::Write(uint16_t address, uint8_t data)
{
    Tick();
    mainBus.Write(address, data);
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
