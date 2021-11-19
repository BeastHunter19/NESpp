#include "CPU.h"
#include "NES.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sys/types.h>

CPU::CPU(NES& mainBus)
    : mainBus(mainBus)
{
    cycleCount = 0;

    PS.value = 0x34;
    A = X = Y = 0;
    // SP value after reset will be 0xFD
    SP = 0x00;

    /*
    // TODO: should be done by peripherals themselves
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    for (uint16_t lastBits = 0x0000; lastBits <= 0x0013; lastBits++)
    {
        uint16_t address = 0x4000 & lastBits;
        mainBus.Write(address, 0x00);
    }
    // TODO: initialize APU registers
    */

    // Fill all opcodes with Illegal dummy instruction to avoid crashes
    opcodeTable.fill({&CPU::Illegal, "Illegal", IMP, 1, 2});

    // ADC
    opcodeTable[0x69] = {&CPU::ADC<&CPU::Immediate>, "ADC", IMM, 2, 2};
    opcodeTable[0x65] = {&CPU::ADC<&CPU::ZeroPage>, "ADC", ZP, 2, 3};
    opcodeTable[0x75] = {&CPU::ADC<&CPU::ZeroPageX>, "ADC", ZPX, 2, 4};
    opcodeTable[0x6D] = {&CPU::ADC<&CPU::Absolute>, "ADC", ABS, 3, 4};
    opcodeTable[0x7D] = {&CPU::ADC<&CPU::AbsoluteX>, "ADC", ABSX, 3, 4, true};
    opcodeTable[0x79] = {&CPU::ADC<&CPU::AbsoluteY>, "ADC", ABSY, 3, 4, true};
    opcodeTable[0x61] = {&CPU::ADC<&CPU::IndexedIndirect>, "ADC", INDX, 2, 6};
    opcodeTable[0x71] = {&CPU::ADC<&CPU::IndirectIndexed>, "ADC", INDY, 2, 5, true};

    // AND
    opcodeTable[0x29] = {&CPU::AND<&CPU::Immediate>, "AND", IMM, 2, 2};
    opcodeTable[0x25] = {&CPU::AND<&CPU::ZeroPage>, "AND", ZP, 2, 3};
    opcodeTable[0x35] = {&CPU::AND<&CPU::ZeroPageX>, "AND", ZPX, 2, 4};
    opcodeTable[0x2D] = {&CPU::AND<&CPU::Absolute>, "AND", ABS, 3, 4};
    opcodeTable[0x3D] = {&CPU::AND<&CPU::AbsoluteX>, "AND", ABSX, 3, 4, true};
    opcodeTable[0x39] = {&CPU::AND<&CPU::AbsoluteY>, "AND", ABSY, 3, 4, true};
    opcodeTable[0x21] = {&CPU::AND<&CPU::IndexedIndirect>, "AND", INDX, 2, 6};
    opcodeTable[0x31] = {&CPU::AND<&CPU::IndirectIndexed>, "AND", INDY, 2, 5, true};

    // ASL
    opcodeTable[0x0A] = {&CPU::ASL<&CPU::Accumulator>, "ASL", ACC, 1, 2};
    opcodeTable[0x06] = {&CPU::ASL<&CPU::ZeroPage>, "ASL", ZP, 2, 5};
    opcodeTable[0x16] = {&CPU::ASL<&CPU::ZeroPageX>, "ASL", ZPX, 2, 6};
    opcodeTable[0x0E] = {&CPU::ASL<&CPU::Absolute>, "ASL", ABS, 3, 6};
    opcodeTable[0x1E] = {&CPU::ASL<&CPU::AbsoluteX>, "ASL", ABSX, 3, 7};

    // BCC
    opcodeTable[0x90] = {&CPU::BCC<&CPU::Relative>, "BCC", REL, 2, 2, true};

    // BCS
    opcodeTable[0xB0] = {&CPU::BCS<&CPU::Relative>, "BCS", REL, 2, 2, true};

    // BEQ
    opcodeTable[0xF0] = {&CPU::BEQ<&CPU::Relative>, "BEQ", REL, 2, 2, true};

    // BIT
    opcodeTable[0x24] = {&CPU::BIT<&CPU::ZeroPage>, "BIT", ZP, 2, 3};
    opcodeTable[0x2C] = {&CPU::BIT<&CPU::Absolute>, "BIT", ABS, 3, 4};

    // BMI
    opcodeTable[0x30] = {&CPU::BMI<&CPU::Relative>, "BMI", REL, 2, 2, true};

    // BNE
    opcodeTable[0xD0] = {&CPU::BNE<&CPU::Relative>, "BNE", REL, 2, 2, true};

    // BPL
    opcodeTable[0x10] = {&CPU::BPL<&CPU::Relative>, "BPL", REL, 2, 2, true};

    // BRK
    opcodeTable[0x00] = {&CPU::BRK<&CPU::Implied>, "BRK", IMP, 1, 7};

    // BVC
    opcodeTable[0x50] = {&CPU::BVC<&CPU::Relative>, "BVC", REL, 2, 2, true};

    // BVS
    opcodeTable[0x70] = {&CPU::BVS<&CPU::Relative>, "BVS", REL, 2, 2, true};

    // CLC
    opcodeTable[0x18] = {&CPU::CLC<&CPU::Implied>, "CLC", IMP, 1, 2};

    // CLD
    opcodeTable[0xD8] = {&CPU::CLD<&CPU::Implied>, "CLD", IMP, 1, 2};

    // CLI
    opcodeTable[0x58] = {&CPU::CLI<&CPU::Implied>, "CLI", IMP, 1, 2};

    // CLV
    opcodeTable[0xB8] = {&CPU::CLV<&CPU::Implied>, "CLV", IMP, 1, 2};

    // CMP
    opcodeTable[0xC9] = {&CPU::CMP<&CPU::Immediate>, "CMP", IMM, 2, 2};
    opcodeTable[0xC5] = {&CPU::CMP<&CPU::ZeroPage>, "CMP", ZP, 2, 3};
    opcodeTable[0xD5] = {&CPU::CMP<&CPU::ZeroPageX>, "CMP", ZPX, 2, 4};
    opcodeTable[0xCD] = {&CPU::CMP<&CPU::Absolute>, "CMP", ABS, 3, 4};
    opcodeTable[0xDD] = {&CPU::CMP<&CPU::AbsoluteX>, "CMP", ABSX, 3, 4, true};
    opcodeTable[0xD9] = {&CPU::CMP<&CPU::AbsoluteY>, "CMP", ABSY, 3, 4, true};
    opcodeTable[0xC1] = {&CPU::CMP<&CPU::IndexedIndirect>, "CMP", INDX, 2, 6};
    opcodeTable[0xD1] = {&CPU::CMP<&CPU::IndirectIndexed>, "CMP", INDY, 2, 5, true};

    // CPX
    opcodeTable[0xE0] = {&CPU::CPX<&CPU::Immediate>, "CPX", IMM, 2, 2};
    opcodeTable[0xE4] = {&CPU::CPX<&CPU::ZeroPage>, "CPX", ZP, 2, 3};
    opcodeTable[0xEC] = {&CPU::CPX<&CPU::Absolute>, "CPX", ABS, 3, 4};

    // CPY
    opcodeTable[0xC0] = {&CPU::CPY<&CPU::Immediate>, "CPY", IMM, 2, 2};
    opcodeTable[0xC4] = {&CPU::CPY<&CPU::ZeroPage>, "CPY", ZP, 2, 3};
    opcodeTable[0xCC] = {&CPU::CPY<&CPU::Absolute>, "CPY", ABS, 3, 4};

    // DEC
    opcodeTable[0xC6] = {&CPU::DEC<&CPU::ZeroPage>, "DEC", ZP, 2, 5};
    opcodeTable[0xD6] = {&CPU::DEC<&CPU::ZeroPageX>, "DEC", ZPX, 2, 6};
    opcodeTable[0xCE] = {&CPU::DEC<&CPU::Absolute>, "DEC", ABS, 3, 6};
    opcodeTable[0xDE] = {&CPU::DEC<&CPU::AbsoluteX>, "DEC", ABSX, 3, 7};

    // DEX
    opcodeTable[0xCA] = {&CPU::DEX<&CPU::Immediate>, "DEX", IMM, 1, 2};

    // DEY
    opcodeTable[0x88] = {&CPU::DEY<&CPU::Immediate>, "DEY", IMM, 1, 2};

    // EOR
    opcodeTable[0x49] = {&CPU::EOR<&CPU::Immediate>, "EOR", IMM, 2, 2};
    opcodeTable[0x45] = {&CPU::EOR<&CPU::ZeroPage>, "EOR", ZP, 2, 3};
    opcodeTable[0x55] = {&CPU::EOR<&CPU::ZeroPageX>, "EOR", ZPX, 2, 4};
    opcodeTable[0x4D] = {&CPU::EOR<&CPU::Absolute>, "EOR", ABS, 3, 4};
    opcodeTable[0x5D] = {&CPU::EOR<&CPU::AbsoluteX>, "EOR", ABSX, 3, 4, true};
    opcodeTable[0x59] = {&CPU::EOR<&CPU::AbsoluteY>, "EOR", ABSY, 3, 4, true};
    opcodeTable[0x41] = {&CPU::EOR<&CPU::IndexedIndirect>, "EOR", INDX, 2, 6};
    opcodeTable[0x51] = {&CPU::EOR<&CPU::IndirectIndexed>, "EOR", INDY, 2, 5, true};

    // INC
    opcodeTable[0xE6] = {&CPU::INC<&CPU::ZeroPage>, "INC", ZP, 2, 5};
    opcodeTable[0xF6] = {&CPU::INC<&CPU::ZeroPageX>, "INC", ZPX, 2, 6};
    opcodeTable[0xEE] = {&CPU::INC<&CPU::Absolute>, "INC", ABS, 3, 6};
    opcodeTable[0xFE] = {&CPU::INC<&CPU::AbsoluteX>, "INC", ABSX, 3, 7};

    // INX
    opcodeTable[0xE8] = {&CPU::INX<&CPU::Immediate>, "INX", IMM, 1, 2};

    // INY
    opcodeTable[0xC8] = {&CPU::INY<&CPU::Immediate>, "INY", IMM, 1, 2};

    // JMP
    opcodeTable[0x4C] = {&CPU::JMP<&CPU::Absolute>, "JMP", ABS, 3, 3};
    opcodeTable[0x6C] = {&CPU::JMP<&CPU::Indirect>, "JMP", IND, 3, 5};

    // JSR
    opcodeTable[0x20] = {&CPU::JSR<&CPU::Absolute>, "JSR", ABS, 3, 6};

    // LDA
    opcodeTable[0xA9] = {&CPU::LDA<&CPU::Immediate>, "LDA", IMM, 2, 2};
    opcodeTable[0xA5] = {&CPU::LDA<&CPU::ZeroPage>, "LDA", ZP, 2, 3};
    opcodeTable[0xB5] = {&CPU::LDA<&CPU::ZeroPageX>, "LDA", ZPX, 2, 4};
    opcodeTable[0xAD] = {&CPU::LDA<&CPU::Absolute>, "LDA", ABS, 3, 4};
    opcodeTable[0xBD] = {&CPU::LDA<&CPU::AbsoluteX>, "LDA", ABSX, 3, 4, true};
    opcodeTable[0xB9] = {&CPU::LDA<&CPU::AbsoluteY>, "LDA", ABSY, 3, 4, true};
    opcodeTable[0xA1] = {&CPU::LDA<&CPU::IndexedIndirect>, "LDA", INDX, 2, 6};
    opcodeTable[0xB1] = {&CPU::LDA<&CPU::IndirectIndexed>, "LDA", INDY, 2, 5, true};

    // LDX
    opcodeTable[0xA2] = {&CPU::LDX<&CPU::Immediate>, "LDX", IMM, 2, 2};
    opcodeTable[0xA6] = {&CPU::LDX<&CPU::ZeroPage>, "LDX", ZP, 2, 3};
    opcodeTable[0xB6] = {&CPU::LDX<&CPU::ZeroPageY>, "LDX", ZPY, 2, 4};
    opcodeTable[0xAE] = {&CPU::LDX<&CPU::Absolute>, "LDX", ABS, 3, 4};
    opcodeTable[0xBE] = {&CPU::LDX<&CPU::AbsoluteY>, "LDX", ABSY, 3, 4, true};

    // LDY
    opcodeTable[0xA0] = {&CPU::LDY<&CPU::Immediate>, "LDY", IMM, 2, 2};
    opcodeTable[0xA4] = {&CPU::LDY<&CPU::ZeroPage>, "LDY", ZP, 2, 3};
    opcodeTable[0xB4] = {&CPU::LDY<&CPU::ZeroPageX>, "LDY", ZPX, 2, 4};
    opcodeTable[0xAC] = {&CPU::LDY<&CPU::Absolute>, "LDY", ABS, 3, 4};
    opcodeTable[0xBC] = {&CPU::LDY<&CPU::AbsoluteX>, "LDY", ABSX, 3, 4, true};

    // LSR
    opcodeTable[0x4A] = {&CPU::LSR<&CPU::Accumulator>, "LSR", ACC, 1, 2};
    opcodeTable[0x46] = {&CPU::LSR<&CPU::ZeroPage>, "LSR", ZP, 2, 5};
    opcodeTable[0x56] = {&CPU::LSR<&CPU::ZeroPageX>, "LSR", ZPX, 2, 6};
    opcodeTable[0x4E] = {&CPU::LSR<&CPU::Absolute>, "LSR", ABS, 3, 6};
    opcodeTable[0x5E] = {&CPU::LSR<&CPU::AbsoluteX>, "LSR", ABSX, 3, 7};

    // NOP
    opcodeTable[0xEA] = {&CPU::NOP<&CPU::Implied>, "NOP", IMP, 1, 2};

    // ORA
    opcodeTable[0x09] = {&CPU::ORA<&CPU::Immediate>, "ORA", IMM, 2, 2};
    opcodeTable[0x05] = {&CPU::ORA<&CPU::ZeroPage>, "ORA", ZP, 2, 3};
    opcodeTable[0x15] = {&CPU::ORA<&CPU::ZeroPageX>, "ORA", ZPX, 2, 4};
    opcodeTable[0x0D] = {&CPU::ORA<&CPU::Absolute>, "ORA", ABS, 3, 4};
    opcodeTable[0x1D] = {&CPU::ORA<&CPU::AbsoluteX>, "ORA", ABSX, 3, 4, true};
    opcodeTable[0x19] = {&CPU::ORA<&CPU::AbsoluteY>, "ORA", ABSY, 3, 4, true};
    opcodeTable[0x01] = {&CPU::ORA<&CPU::IndexedIndirect>, "ORA", INDX, 2, 6};
    opcodeTable[0x11] = {&CPU::ORA<&CPU::IndirectIndexed>, "ORA", INDY, 2, 5, true};

    // PHA
    opcodeTable[0x48] = {&CPU::PHA<&CPU::Implied>, "PHA", IMP, 1, 3};

    // PHP
    opcodeTable[0x08] = {&CPU::PHP<&CPU::Implied>, "PHP", IMP, 1, 3};

    // PLA
    opcodeTable[0x68] = {&CPU::PLA<&CPU::Implied>, "PLA", IMP, 1, 4};

    // PLP
    opcodeTable[0x28] = {&CPU::PLP<&CPU::Implied>, "PLP", IMP, 1, 4};

    // ROL
    opcodeTable[0x2A] = {&CPU::ROL<&CPU::Accumulator>, "ROL", ACC, 1, 2};
    opcodeTable[0x26] = {&CPU::ROL<&CPU::ZeroPage>, "ROL", ZP, 2, 5};
    opcodeTable[0x36] = {&CPU::ROL<&CPU::ZeroPageX>, "ROL", ZPX, 2, 6};
    opcodeTable[0x2E] = {&CPU::ROL<&CPU::Absolute>, "ROL", ABS, 3, 6};
    opcodeTable[0x3E] = {&CPU::ROL<&CPU::AbsoluteX>, "ROL", ABSX, 3, 7};

    // ROR
    opcodeTable[0x6A] = {&CPU::ROR<&CPU::Accumulator>, "ROR", ACC, 1, 2};
    opcodeTable[0x66] = {&CPU::ROR<&CPU::ZeroPage>, "ROR", ZP, 2, 5};
    opcodeTable[0x76] = {&CPU::ROR<&CPU::ZeroPageX>, "ROR", ZPX, 2, 6};
    opcodeTable[0x6E] = {&CPU::ROR<&CPU::Absolute>, "ROR", ABS, 3, 6};
    opcodeTable[0x7E] = {&CPU::ROR<&CPU::AbsoluteX>, "ROR", ABSX, 3, 7};

    // RTI
    opcodeTable[0x40] = {&CPU::RTI<&CPU::Implied>, "RTI", IMP, 1, 6};

    // RTS
    opcodeTable[0x60] = {&CPU::RTS<&CPU::Implied>, "RTS", IMP, 1, 6};

    // SBC
    opcodeTable[0xE9] = {&CPU::SBC<&CPU::Immediate>, "SBC", IMM, 2, 2};
    opcodeTable[0xE5] = {&CPU::SBC<&CPU::ZeroPage>, "SBC", ZP, 2, 3};
    opcodeTable[0xF5] = {&CPU::SBC<&CPU::ZeroPageX>, "SBC", ZPX, 2, 4};
    opcodeTable[0xED] = {&CPU::SBC<&CPU::Absolute>, "SBC", ABS, 3, 4};
    opcodeTable[0xFD] = {&CPU::SBC<&CPU::AbsoluteX>, "SBC", ABSX, 3, 4, true};
    opcodeTable[0xF9] = {&CPU::SBC<&CPU::AbsoluteY>, "SBC", ABSY, 3, 4, true};
    opcodeTable[0xE1] = {&CPU::SBC<&CPU::IndexedIndirect>, "SBC", INDX, 2, 6};
    opcodeTable[0xF1] = {&CPU::SBC<&CPU::IndirectIndexed>, "SBC", INDY, 2, 5, true};

    // SEC
    opcodeTable[0x38] = {&CPU::SEC<&CPU::Implied>, "SEC", IMP, 1, 2};

    // SED
    opcodeTable[0xF8] = {&CPU::SED<&CPU::Implied>, "SED", IMP, 1, 2};

    // SEI
    opcodeTable[0x78] = {&CPU::SEI<&CPU::Implied>, "SEI", IMP, 1, 2};

    // STA
    opcodeTable[0x85] = {&CPU::STA<&CPU::ZeroPage>, "STA", ZP, 2, 3};
    opcodeTable[0x95] = {&CPU::STA<&CPU::ZeroPageX>, "STA", ZPX, 2, 4};
    opcodeTable[0x8D] = {&CPU::STA<&CPU::Absolute>, "STA", ABS, 3, 4};
    opcodeTable[0x9D] = {&CPU::STA<&CPU::AbsoluteX>, "STA", ABSX, 3, 5};
    opcodeTable[0x99] = {&CPU::STA<&CPU::AbsoluteY>, "STA", ABSY, 3, 5};
    opcodeTable[0x81] = {&CPU::STA<&CPU::IndexedIndirect>, "STA", INDX, 2, 6};
    opcodeTable[0x91] = {&CPU::STA<&CPU::IndirectIndexed>, "STA", INDY, 2, 6};

    // STX
    opcodeTable[0x86] = {&CPU::STX<&CPU::ZeroPage>, "STX", ZP, 2, 3};
    opcodeTable[0x96] = {&CPU::STX<&CPU::ZeroPageY>, "STX", ZPY, 2, 4};
    opcodeTable[0x8E] = {&CPU::STX<&CPU::Absolute>, "STX", ABS, 3, 4};

    // STY
    opcodeTable[0x84] = {&CPU::STY<&CPU::ZeroPage>, "STY", ZP, 2, 3};
    opcodeTable[0x94] = {&CPU::STY<&CPU::ZeroPageX>, "STY", ZPX, 2, 4};
    opcodeTable[0x8C] = {&CPU::STY<&CPU::Absolute>, "STY", ABS, 3, 4};

    // TAX
    opcodeTable[0xAA] = {&CPU::TAX<&CPU::Implied>, "TAX", IMP, 1, 2};

    // TAY
    opcodeTable[0xA8] = {&CPU::TAY<&CPU::Implied>, "TAY", IMP, 1, 2};

    // TSX
    opcodeTable[0xBA] = {&CPU::TSX<&CPU::Implied>, "TSX", IMP, 1, 2};

    // TXA
    opcodeTable[0x8A] = {&CPU::TXA<&CPU::Implied>, "TXA", IMP, 1, 2};

    // TXS
    opcodeTable[0x9A] = {&CPU::TXS<&CPU::Implied>, "TXS", IMP, 1, 2};

    // TYA
    opcodeTable[0x98] = {&CPU::TYA<&CPU::Implied>, "TYA", IMP, 1, 2};
}

void CPU::ExecuteInstrFromRAM(uint16_t startingLocation, size_t number)
{
    cycleCount = 0;
    PC = startingLocation;
    while (PC < (startingLocation + number) && PC >= startingLocation && cycleCount < CYCLES_PER_FRAME)
    {
        opcode = Read(PC++);
        ExecuteInstruction();
    }
}

void CPU::Run()
{
    static std::ofstream log("bbNESlog.txt");
    do
    {
        log << std::hex << std::uppercase << (int)PC << '\t';
        opcode = Read(PC);
        for(int i = 0; i < opcodeTable[opcode].bytes; i++)
        {
            log << std::hex << (int)mainBus.Read(PC + i) << ' ';
        }
        PC++;
        log << "\t\t" << opcodeTable[opcode].mnemonic << "\t\t\tA:" << std::hex << (int)A << " X:" << (int)X << " Y:" << (int)Y << " PS:" << (int)PS.value << " SP:" << (int)SP << " cycles:" << cycleCount << '\n';
        ExecuteInstruction();

    } while(opcode != 0x00 && opcodeTable[opcode].ptr != &CPU::Illegal);
}

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
    std::cout << "Executed instruction: " << opcodeTable[opcode].mnemonic << '\n';
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

void CPU::Branch(bool condition, int extraCycle)
{
    if (condition == true)
    {
        if (extraCycle == 1)
        {
            Tick();
        }
        Tick();
        PC += address;
    }
}

void CPU::PushStack(uint8_t value)
{
    Write(0x0100 + SP, value);
    SP--;
}

uint8_t CPU::PullStack()
{
    SP++;
    return Read(0x0100 + SP);
}

void CPU::Compare(uint8_t reg, uint8_t operand)
{
    if (reg >= operand)
    {
        PS.Set<C>();
    }
    else
    {
        PS.Clear<C>();
    }
    if (reg == operand)
    {
        PS.Set<Z>();
    }
    else
    {
        PS.Clear<Z>();
    }
    uint8_t memBit7 = ((reg - operand) & 0x80) >> 7;
    PS.Assign<N>(memBit7);
}

// ADDDRESSING MODES

int CPU::Implied()
{
    return 0;
}

int CPU::Accumulator()
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

int CPU::Relative()
{
    uint8_t offset = Read(PC++);
    address = offset;
    if (PageCrossed(PC, offset))
    {
        return 1;
    }
    else
    {
        return 0;
    }
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
    zeroPageAddress += X;
    address = (uint16_t)zeroPageAddress;
    return 0;
}

int CPU::ZeroPageY()
{
    uint8_t zeroPageAddress = Read(PC++);
    Tick();
    zeroPageAddress += Y;
    address = (uint16_t)zeroPageAddress;
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
    if((this->*AddrMode)() == 1)
    {
        Tick();
    }
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
    if ((~(A ^ operand) & (A ^ result)) & 0x80)
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
void CPU::AND()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    A &= operand;
    UpdateZN(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::ASL()
{
    if (AddrMode == &CPU::Accumulator)
    {
        Tick();
        uint8_t accBit7 = (A & 0x80) >> 7;
        PS.Assign<C>(accBit7);
        A = A << 1;
        UpdateZN(A);
    }
    else
    {
        (this->*AddrMode)();
        uint8_t operand = Read(address);
        Write(address, operand);
        uint8_t memBit7 = (operand & 0x80) >> 7;
        PS.Assign<C>(memBit7);
        operand = operand << 1;
        UpdateZN(operand);
        Write(address, operand);
    }
}

template <CPU::AddressModePtr AddrMode>
void CPU::BCC()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<C>() == 0, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BCS()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<C>() == 1, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BEQ()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<Z>() == 1, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BIT()
{
    (this->*AddrMode)();
    uint8_t operand = Read(address);
    uint8_t result = A & operand;
    if (result == 0)
    {
        PS.Set<Z>();
    }
    else
    {
        PS.Clear<Z>();
    }
    uint8_t memBit6 = (operand & 0x40) >> 6;
    uint8_t memBit7 = (operand & 0x80) >> 7;
    PS.Assign<V>(memBit6);
    PS.Assign<N>(memBit7);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BMI()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<N>() == 1, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BNE()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<Z>() == 0, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BPL()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<N>() == 0, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BRK()
{
    Tick();
    PC++;
    uint8_t PCH = (PC & 0xFF00) >> 8;
    uint8_t PCL = PC & 0x00FF;
    PushStack(PCH);
    PushStack(PCL);
    PS.Set<B>();
    PushStack(PS.value);
    PS.Set<I>();
    PCL = Read(0xFFFE);
    PCH = Read(0xFFFF);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BVC()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<V>() == 0, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::BVS()
{
    int extraCycle = (this->*AddrMode)();
    Branch(PS.Test<V>() == 1, extraCycle);
}

template <CPU::AddressModePtr AddrMode>
void CPU::CLC()
{
    Tick();
    PS.Clear<C>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::CLD()
{
    Tick();
    PS.Clear<D>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::CLI()
{
    Tick();
    PS.Clear<I>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::CLV()
{
    Tick();
    PS.Clear<V>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::CMP()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    Compare(A, operand);
}

template <CPU::AddressModePtr AddrMode>
void CPU::CPX()
{
    (this->*AddrMode)();
    uint8_t operand = Read(address);
    Compare(X, operand);
}

template <CPU::AddressModePtr AddrMode>
void CPU::CPY()
{
    (this->*AddrMode)();
    uint8_t operand = Read(address);
    Compare(Y, operand);
}

template <CPU::AddressModePtr AddrMode>
void CPU::DEC()
{
    (this->*AddrMode)();
    if (AddrMode == &CPU::AbsoluteX)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    Write(address, operand);
    operand--;
    UpdateZN(operand);
    Write(address, operand);
}

template <CPU::AddressModePtr AddrMode>
void CPU::DEX()
{
    Tick();
    X--;
    UpdateZN(X);
}

template <CPU::AddressModePtr AddrMode>
void CPU::DEY()
{
    Tick();
    Y--;
    UpdateZN(Y);
}

template <CPU::AddressModePtr AddrMode>
void CPU::EOR()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    A ^= operand;
    UpdateZN(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::INC()
{
    (this->*AddrMode)();
    if (AddrMode == &CPU::AbsoluteX)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    Write(address, operand);
    operand++;
    UpdateZN(operand);
    Write(address, operand);
}

template <CPU::AddressModePtr AddrMode>
void CPU::INX()
{
    Tick();
    X++;
    UpdateZN(X);
}

template <CPU::AddressModePtr AddrMode>
void CPU::INY()
{
    Tick();
    Y++;
    UpdateZN(Y);
}

template <CPU::AddressModePtr AddrMode>
void CPU::JMP()
{
    (this->*AddrMode)();
    PC = address;
}

template <CPU::AddressModePtr AddrMode>
void CPU::JSR()
{
    // this shoul use absolute addressing, but it's
    // actually a bit different
    uint8_t addressLow = Read(PC++);
    Tick();
    uint8_t PCH = (PC & 0xFF00) >> 8;
    uint8_t PCL = PC & 0x00FF;
    PushStack(PCH);
    PushStack(PCL);
    PCL = addressLow;
    PCH = Read(PC);
    PC = ((uint16_t)PCH << 8) | PCL;
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
void CPU::LSR()
{
    if (AddrMode == &CPU::Accumulator)
    {
        Tick();
        uint8_t accBit0 = A & 0x01;
        PS.Assign<C>(accBit0);
        A = A >> 1;
        UpdateZN(A);
    }
    else
    {
        (this->*AddrMode)();
        uint8_t operand = Read(address);
        Write(address, operand);
        uint8_t memBit0 = operand & 0x01;
        PS.Assign<C>(memBit0);
        operand = operand >> 1;
        UpdateZN(operand);
        Write(address, operand);
    }
}

template <CPU::AddressModePtr AddrMode>
void CPU::NOP()
{
    Tick();
}

template <CPU::AddressModePtr AddrMode>
void CPU::ORA()
{
    if ((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    A |= operand;
    UpdateZN(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::PHA()
{
    Tick();
    PushStack(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::PHP()
{
    Tick();
    PushStack(PS.value);
}

template <CPU::AddressModePtr AddrMode>
void CPU::PLA()
{
    Tick();
    Tick();
    A = PullStack();
    UpdateZN(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::PLP()
{
    Tick();
    Tick();
    PS.value = PullStack();
}

template <CPU::AddressModePtr AddrMode>
void CPU::ROL()
{
    if (AddrMode == &CPU::Accumulator)
    {
        Tick();
        uint8_t accBit7 = (A & 0x80) >> 7;
        A = A << 1;
        A |= PS.Test<C>();
        PS.Assign<C>(accBit7);
        UpdateZN(A);
    }
    else
    {
        (this->*AddrMode)();
        uint8_t operand = Read(address);
        Write(address, operand);
        uint8_t memBit7 = (operand & 0x80) >> 7;
        operand = operand << 1;
        operand |= PS.Test<C>();
        PS.Assign<C>(memBit7);
        UpdateZN(operand);
        Write(address, operand);
    }
}

template <CPU::AddressModePtr AddrMode>
void CPU::ROR()
{
    if (AddrMode == &CPU::Accumulator)
    {
        Tick();
        uint8_t accBit0 = A & 0x01;
        A = A >> 1;
        A |= (PS.Test<C>() << 7);
        PS.Assign<C>(accBit0);
        UpdateZN(A);
    }
    else
    {
        (this->*AddrMode)();
        uint8_t operand = Read(address);
        Write(address, operand);
        uint8_t memBit0 = operand & 0x01;
        operand = operand >> 1;
        operand |= (PS.Test<C>() << 7);
        PS.Assign<C>(memBit0);
        UpdateZN(operand);
        Write(address, operand);
    }
}

template<CPU::AddressModePtr AddrMode>
void CPU::RTI()
{
    Tick();
    Tick();
    PS.value = PullStack();
    uint8_t PCL = PullStack();
    uint8_t PCH = PullStack();
    PC = ((uint16_t)PCH << 8) | PCL;
}

template<CPU::AddressModePtr AddrMode>
void CPU::RTS()
{
    Tick();
    Tick();
    uint8_t PCL = PullStack();
    uint8_t PCH = PullStack();
    PC = ((uint16_t)PCH << 8) | PCL;
    Tick();
    PC++;
}

template<CPU::AddressModePtr AddrMode>
void CPU::SBC()
{
    if((this->*AddrMode)() == 1)
    {
        Tick();
    }
    uint8_t operand = Read(address);
    uint8_t carry = PS.Test<C>();
    // operand's sign is changed, as in the actual 6502,
    // by taking it's two's complement: it is first complemented
    // and then 1 is added; actually the carry bit is added since
    // it is always assumed that it should be set to 1 before doing
    // arithmetic subtractions. If there actually was a borrow in
    // the previous subtraction the carry bit would be 0 and this
    // basically means that 1 is subtracted from the final result
    // (since 0 is added instead of 1 when taking two's complement)
    operand = ~operand + carry;
    uint8_t result = A + operand;
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
    if ((~(A ^ operand) & (A ^ result)) & 0x80)
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
void CPU::SEC()
{
    Tick();
    PS.Set<C>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::SED()
{
    Tick();
    PS.Set<D>();
}

template <CPU::AddressModePtr AddrMode>
void CPU::SEI()
{
    Tick();
    PS.Set<I>();
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

template <CPU::AddressModePtr AddrMode>
void CPU::STX()
{
    (this->*AddrMode)();
    Write(address, X);
}

template <CPU::AddressModePtr AddrMode>
void CPU::STY()
{
    (this->*AddrMode)();
    Write(address, Y);
}

template <CPU::AddressModePtr AddrMode>
void CPU::TAX()
{
    Tick();
    X = A;
    UpdateZN(X);
}

template <CPU::AddressModePtr AddrMode>
void CPU::TAY()
{
    Tick();
    Y = A;
    UpdateZN(Y);
}

template <CPU::AddressModePtr AddrMode>
void CPU::TSX()
{
    Tick();
    X = SP;
    UpdateZN(X);
}

template <CPU::AddressModePtr AddrMode>
void CPU::TXA()
{
    Tick();
    A = X;
    UpdateZN(A);
}

template <CPU::AddressModePtr AddrMode>
void CPU::TXS()
{
    Tick();
    SP = X;
}

template <CPU::AddressModePtr AddrMode>
void CPU::TYA()
{
    Tick();
    A = Y;
    UpdateZN(A);
}
