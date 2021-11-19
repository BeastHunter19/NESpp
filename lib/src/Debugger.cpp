#include "Debugger.h"
#include "EmulatorCore.h"
#include <fmt/core.h>
#include <stdexcept>
#include <fstream>

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

void Debugger::SetPC(uint16_t address)
{
    core->cpu.PC = address;
}

bool Debugger::LoadROM(const std::string& pathToROM)
{
    return core->LoadGame(pathToROM);
}

void Debugger::RunWithTrace(const std::filesystem::path& output)
{
    static std::ofstream log(output);
    std::string disassembly, outputLine;
    const CPU::Instruction* instr;
    CpuState state;
    do
    {
        core->cpu.opcode = core->cpu.Read(core->cpu.PC);
        instr = &(core->cpu.opcodeTable[core->cpu.opcode]);
        Disassembly(&disassembly, core->cpu.PC, instr->bytes);
        state = GetCpuState();
        outputLine = fmt::format("{}\t\t\t\tA:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}\tcycles:{:d}\n",
                                 disassembly, state.A, state.X, state.Y, state.PS.value, state.SP, state.cycleCount);
        log << outputLine;
        core->cpu.PC++;
        core->cpu.ExecuteInstruction();
    } while(core->cpu.opcode != 0x00 && instr->ptr != &CPU::Illegal);}

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
                fmt::format("{:0>4X} {:02X}\t\t\t{}", address, bytes[0], currentInstruction.mnemonic);
            break;
        }
        case CPU::IMM: {
            outputArray[instructionsRead] = fmt::format("{:0>4X} {:02X} {:02X}\t\t\t{} #${:0>2X}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZP: {
            outputArray[instructionsRead] = fmt::format("{:0>4X} {:02X} {:02X}\t\t\t{} ${:0>2X}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZPX: {
            outputArray[instructionsRead] = fmt::format("{:0>4X} {:02X} {:02X}\t\t\t{} ${:0>2X},x", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ZPY: {
            outputArray[instructionsRead] = fmt::format("{:0>4X} {:02X} {:02X}\t\t\t{} ${:0>2X},y", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, bytes[1]);
            break;
        }
        case CPU::ABS: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} ${:0>2X}{:0>2X}", address, bytes[0], bytes[1], bytes[2],
                            currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::ABSX: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} ${:0>2X}{:0>2X},x", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::ABSY: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} ${:0>2X}{:0>2X},y", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::REL: {
            outputArray[instructionsRead] = fmt::format("{:0>4X} {:02X} {:02X}\t\t\t{} ${:+d}", address, bytes[0],
                                                        bytes[1], currentInstruction.mnemonic, (int)bytes[1]);
            break;
        }
        case CPU::IND: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} (${:0>2X}{:0>2X})", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::INDX: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} (${:0>2X}{:0>2X},x)", address, bytes[0], bytes[1],
                            bytes[2], currentInstruction.mnemonic, bytes[2], bytes[1]);
            break;
        }
        case CPU::INDY: {
            outputArray[instructionsRead] =
                fmt::format("{:0>4X} {:02X} {:02X} {:02X}\t\t\t{} (${:0>2X}{:0>2X}),y", address, bytes[0], bytes[1],
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
    core->cpu.Reset();
    LoadInstrFromArray(instructions, number, startingLocation);
    core->cpu.ExecuteInstrFromRAM(startingLocation, number);
    return GetCpuState();
}

const std::array<uint8_t, 2048>& Debugger::GetMemoryState() const
{
    return core->RAM;
}

Debugger::CpuState Debugger::GetCpuState() const
{
    return {core->cpu.SP, core->cpu.A, core->cpu.X, core->cpu.Y, core->cpu.PS, core->cpu.PC, core->cpu.cycleCount};
}
