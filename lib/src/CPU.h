#ifndef CPU_H
#define CPU_H

#include "BitMappedRegister.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

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
    CPU(class NES& mainBus);

    friend class Debugger;

    void ExecuteInstrFromRAM(uint16_t startingLocation, size_t number);

    void Run();

    // Must be called before starting execution
    void Reset();

    void ExecuteInstruction();

    typedef int (CPU::*AddressModePtr)();

    typedef void (CPU::*InstructionPtr)();

    enum AddressingMode
    {
        IMP,
        ACC,
        IMM,
        ZP,
        ABS,
        REL,
        IND,
        ZPX,
        ZPY,
        ABSX,
        ABSY,
        INDX,
        INDY
    };

    struct Instruction
    {
        InstructionPtr ptr;
        const char* mnemonic;
        AddressingMode mode;
        int bytes;
        int cycles;
        bool extraCycle = false;
    };

    // TODO: try using std::function and a
    // separate class for opcode representation

    std::array<Instruction, 256> opcodeTable;

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

private:
    // Reference to the main bus
    NES& mainBus;

    // Program counter
    uint16_t PC;

    // Stack pointer
    uint8_t SP;

    // General purpose registers
    uint8_t A, X, Y;

    // Processor status
    BitMappedRegister<CpuStatusFlags> PS;

    uint32_t cycleCount;

    // These are used during instruction execution
    uint8_t opcode;
    uint16_t address;

    // Increment cycle count
    inline void Tick();

    // Access the main addressing space
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    // Checks if there is going to be a page boundary cross
    inline bool PageCrossed(uint16_t address, uint16_t offset);

    // Update processor status flags Z and N
    inline void UpdateZN(uint8_t value);

    // Shared logic for branch instructions
    inline void Branch(bool condition, int extraCycle);

    // Access stack (locations 0x0100-0x01FF)
    inline void PushStack(uint8_t value);
    inline uint8_t PullStack();

    // Compares the values setting flags accordingly
    inline void Compare(uint8_t reg, uint8_t operand);

    /*
     * The instructions can have different behavior
     * depending on the addressing mode used, which
     * is the way the operands are fetched from memory.
     * The addressing modes are represented as separate
     * functions that are then called to retrieve the
     * operands for the instruction functions.
     * All the addressing mode functions return the
     * number of extra cycles that they take (if any).
     *
     *
     * ------- Addressing modes -------
     *
     * The following two modes actually do nothing
     * - Implicit: there are no operands
     * - Accumulator: operates on the accumulator
     */

    // The operand is implicit in the instruction
    inline int Implied();

    // The operand is the accumulator
    inline int Accumulator();

    // Uses a 8 bit operand directly without memory access
    inline int Immediate();

    // Fetches the value from 8 bit address from zero page
    inline int ZeroPage();

    // Fetches the value from anywhere in memory
    inline int Absolute();

    // Only used in branch instruction, uses 8 bit offset
    inline int Relative();

    // Only used in jump instruction, uses a pointer in memory
    inline int Indirect();

    // The following modes are indexed: they use X and Y
    // registers to offset addresses obtained through other modes

    // Same ad ZeroPage, but adds value in X register
    inline int ZeroPageX();

    // Same ad ZeroPage, but adds value in Y register
    inline int ZeroPageY();

    // Same ad Absolute, but adds value in X register
    inline int AbsoluteX();

    // Same ad Absolute, but adds value in Y register
    inline int AbsoluteY();

    // Uses a 8 bit address incremented by X to fetch
    // a pointer from zero page
    inline int IndexedIndirect();

    // Uses a 8 bit address to fetch a pointer from
    // zero page, which is then incremented by Y
    inline int IndirectIndexed();

    /*
     * ------- Instructions -------
     *
     * There are 56 different instructions in the 6502,
     * but each one can have multiple addressing modes,
     * thus achieving a total of 151 instructions.
     * The remaining unused or "illegal" opcodes have
     * varying kinds of effects, from useful ones to
     * others that simply jam the machine until it is reset.
     */

    // Empty placeholder instruction for illegal opcodes
    void Illegal();

    template <AddressModePtr AddrMode>
    inline void ADC();

    template <AddressModePtr AddrMode>
    inline void AND();

    template <AddressModePtr AddrMode>
    inline void ASL();

    template <AddressModePtr AddrMode>
    inline void BCC();

    template <AddressModePtr AddrMode>
    inline void BCS();

    template <AddressModePtr AddrMode>
    inline void BEQ();

    template <AddressModePtr AddrMode>
    inline void BIT();

    template <AddressModePtr AddrMode>
    inline void BMI();

    template <AddressModePtr AddrMode>
    inline void BNE();

    template <AddressModePtr AddrMode>
    inline void BPL();

    template <AddressModePtr AddrMode>
    inline void BRK();

    template <AddressModePtr AddrMode>
    inline void BVC();

    template <AddressModePtr AddrMode>
    inline void BVS();

    template <AddressModePtr AddrMode>
    inline void CLC();

    template <AddressModePtr AddrMode>
    inline void CLD();

    template <AddressModePtr AddrMode>
    inline void CLI();

    template <AddressModePtr AddrMode>
    inline void CLV();

    template <AddressModePtr AddrMode>
    inline void CMP();

    template <AddressModePtr AddrMode>
    inline void CPX();

    template <AddressModePtr AddrMode>
    inline void CPY();

    template <AddressModePtr AddrMode>
    inline void DEC();

    template <AddressModePtr AddrMode>
    inline void DEX();

    template <AddressModePtr AddrMode>
    inline void DEY();

    template <AddressModePtr AddrMode>
    inline void EOR();

    template <AddressModePtr AddrMode>
    inline void INC();

    template <AddressModePtr AddrMode>
    inline void INX();

    template <AddressModePtr AddrMode>
    inline void INY();

    template <AddressModePtr AddrMode>
    inline void JMP();

    template <AddressModePtr AddrMode>
    inline void JSR();

    template <AddressModePtr AddrMode>
    inline void LDA();

    template <AddressModePtr AddrMode>
    inline void LDX();

    template <AddressModePtr AddrMode>
    inline void LDY();

    template <AddressModePtr AddrMode>
    inline void LSR();

    template <AddressModePtr AddrMode>
    inline void NOP();

    template <AddressModePtr AddrMode>
    inline void ORA();

    template <AddressModePtr AddrMode>
    inline void PHA();

    template <AddressModePtr AddrMode>
    inline void PHP();

    template <AddressModePtr AddrMode>
    inline void PLA();

    template <AddressModePtr AddrMode>
    inline void PLP();

    template <AddressModePtr AddrMode>
    inline void ROL();

    template <AddressModePtr AddrMode>
    inline void ROR();

    template <AddressModePtr AddrMode>
    inline void RTI();

    template <AddressModePtr AddrMode>
    inline void RTS();

    template <AddressModePtr AddrMode>
    inline void SBC();

    template <AddressModePtr AddrMode>
    inline void SEC();

    template <AddressModePtr AddrMode>
    inline void SED();

    template <AddressModePtr AddrMode>
    inline void SEI();

    template <AddressModePtr AddrMode>
    inline void STA();

    template <AddressModePtr AddrMode>
    inline void STX();

    template <AddressModePtr AddrMode>
    inline void STY();

    template <AddressModePtr AddrMode>
    inline void TAX();

    template <AddressModePtr AddrMode>
    inline void TAY();

    template <AddressModePtr AddrMode>
    inline void TSX();

    template <AddressModePtr AddrMode>
    inline void TXA();

    template <AddressModePtr AddrMode>
    inline void TXS();

    template <AddressModePtr AddrMode>
    inline void TYA();
};

#endif // CPU_H
