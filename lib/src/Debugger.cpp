#include "Debugger.h"
#include "EmulatorCore.h"
#include <fmt/core.h>
#include <stdexcept>

Debugger::Debugger(const EmulatorCore& other)
    : EmulatorCore(other)
{
}

Debugger& Debugger::operator=(const EmulatorCore& other)
{
    *this = EmulatorCore::operator=(other);
    return *this;
}

void Debugger::LoadInstrFromArray(const uint8_t* instructions, size_t number, uint16_t startingLocation)
{
    if (startingLocation + number > 0x7FF)
    {
        throw std::out_of_range("The given program would exceed RAM capacity of 2KB");
    }
    std::memcpy(core->RAM.data() + startingLocation, instructions, number);
}

size_t Debugger::Disassembly(std::string* outputArray, uint16_t startingAddress, size_t number)
{
    size_t address = startingAddress;
    uint8_t bytes[3];
    size_t instructionsRead = 0;
    while (address < startingAddress + number)
    {
        const CPU::Instruction& currentInstruction = core->cpu.opcodeTable[core->Read(address)];
        for (size_t i = 0; i < currentInstruction.bytes; i++)
        {
            bytes[i] = core->Read(address + i);
        }
        switch (currentInstruction.mode)
        {
        case CPU::IMP:
        case CPU::ACC: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X}\t{}", address, bytes[0], currentInstruction.mnemonic);
            break;
        }
        case CPU::IMM: {
            outputArray[instructionsRead] = fmt::format("${:0>4X}: {:02X} {:02X}\t{} #${:0>2X}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZP: {
            outputArray[instructionsRead] = fmt::format("${:0>4X}: {:02X} {:02X}\t{} ${:0>2X}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZPX: {
            outputArray[instructionsRead] = fmt::format("${:0>4X}: {:02X} {:02X}\t{} ${:0>2X},x", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZPY: {
            outputArray[instructionsRead] = fmt::format("${:0>4X}: {:02X} {:02X}\t{} ${:0>2X},y", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ABS: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} ${:0>2X}{:0>2X}", address, bytes[0], bytes[1], bytes[2],
                            currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::ABSX: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} ${:0>2X}{:0>2X},x", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::ABSY: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} ${:0>2X}{:0>2X},y", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::REL: {
            outputArray[instructionsRead] = fmt::format("${:0>4X}: {:02X} {:02X}\t{} ${:+d}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, (int)bytes[1]);
            break;
        }
        case CPU::IND: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} (${:0>2X}{:0>2X})", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::INDX: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} (${:0>2X}{:0>2X},x)", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::INDY: {
            outputArray[instructionsRead] =
                fmt::format("${:0>4X}: {:02X} {:02X} {:02X}\t{} (${:0>2X}{:0>2X}),y", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        }

        address += currentInstruction.bytes;
        instructionsRead++;
    }
    return instructionsRead;
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
