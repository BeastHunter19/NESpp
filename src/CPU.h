#ifndef CPU_H
#define CPU_H

#include "BitMappedRegister.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

/*
 * NES CPU (Ricoh RP2A03) is based on the 6502
 * processor, with an integrated APU for sound
 * processing and without the decimal mode of
 * original MOS6502.
 * It has only three general purpose registers,
 * other than the program counter, stack pointer
 * and status register.
 */

const uint32_t CYCLES_PER_FRAME = 29781;

class CPU
{
public:
    CPU(class NES* mainBus);

    // Assumes the CPU is in a clean state (mostly used for testing)
    void LoadInstrFromArray(const uint8_t* instructions, size_t number);

    void Run();

    // Is automatically called by constructor
    void Reset();

    void ExecuteInstruction(uint8_t opcode);

private:
    // Reference to the main bus
    NES* mainBus;

    // Program counter
    uint16_t PC;

    // Stack pointer
    uint8_t SP;

    // General purpose registers
    uint8_t A, X, Y;

    // Processor status
    enum CpuStatusFlags : uint8_t
    {
        C = 0x01, // carry
        Z = 0x02, // zero
        I = 0x04, // interrupt disable
        D = 0x08, // decimal (unused on NES)
        B = 0x10, // break
        _ = 0x20, // unused
        V = 0x40, // overflow
        N = 0x80, // negative
    };
    BitMappedRegister<CpuStatusFlags> PS;

    uint32_t cycleCount;

    // Increment cycle count
    inline void Tick();

    // Access the main addressing space
    inline uint8_t Read(uint16_t address);
    inline void Write(uint16_t address, uint8_t data);

    /*
     * The instructions can have different behavior
     * depending on the addressing mode used, which
     * is the way the operands are fetched from memory.
     * The addressing modes are represented as separate
     * functions that are then called to retrieve the
     * operands for the instruction functions
     *
     *
     * ------- Addressing modes -------
     *
     * The following two modes actually do nothing
     * - Implicit: there are no operands
     * - Accumulator: operates on the accumulator
     */

    // Uses a 8 bit operand directly without memory access
    inline uint16_t Immediate();

    // Fetches the value from 8 bit address from zero page
    inline uint16_t ZeroPage();

    // Fetches the value from anywhere in memory
    inline uint16_t Absolute();

    // Only used in branch instruction, uses 8 bit offset
    inline uint16_t Relative();

    // Only used in jump instruction, uses a pointer in memory
    inline uint16_t Indirect();

    // The following modes are indexed: they use X and Y
    // registers to offset addresses obtained through other modes

    // Same ad ZeroPage, but adds value in X register
    inline uint16_t ZeroPageX();

    // Same ad ZeroPage, but adds value in Y register
    inline uint16_t ZeroPageY();

    // Same ad Absolute, but adds value in X register
    inline uint16_t AbsoluteX();

    // Same ad Absolute, but adds value in Y register
    inline uint16_t AbsoluteY();

    // Uses a 8 bit address incremented by X to fetch
    // a pointer from zero page
    inline uint16_t IndexedIndirect();

    // Uses a 8 bit address to fetch a pointer from
    // zero page, which is then incremented by Y
    inline uint16_t IndirectIndexed();
};

#endif // CPU_H
