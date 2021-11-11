#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "EmulatorCore.h"

/*
 * Alternative interface for the NES emulator core.
 * Exposes more low level functionality and is
 * more granular than the normal Emulator interface.
 * It's declared as friend by most components, allowing
 * to create a full featured debugger for the console.
 */

class Debugger : public EmulatorCore
{
public:
    // The debugger must be initialized from an already existing
    // object that has a functional emulator core
    Debugger(const EmulatorCore& other)
        : EmulatorCore(other)
    {
    }
    ~Debugger() = default;

    // Assumes the CPU is in a clean state (mostly used for testing).
    // Loads <number> instructions from the given array starting at
    // memory location <startingLocation>
    void LoadInstrFromArray(const uint8_t* instructions, size_t number, uint16_t startingLocation = 0x0700);
    void ShowDisassembly();

    struct CpuState
    {
        uint8_t SP, A, X, Y;
        BitMappedRegister<CPU::CpuStatusFlags> PS;
        uint16_t PC;
        uint32_t cycleCount;
    };
    CpuState GetCpuState() const;

    // Loads and directly executes the whole sequence of instructions in the given array
    CpuState ExecuteInstrFromArray(const uint8_t* instructions, size_t number, uint16_t startingLocation = 0x0700);

    const std::array<uint8_t, 2048>& GetMemoryState() const;
};

#endif // DEBUGGER_H