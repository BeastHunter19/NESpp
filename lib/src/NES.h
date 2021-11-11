#ifndef NES_H
#define NES_H

#include "CPU.h"
#include <array>

/*
 * This class is going to have the role of
 * mediator between the different periferals
 * attached to the NES main bus: the CPU,
 * the APU and all the other components that
 * are controlled through memory mapped IO.
 * The actual memory layout of the bus is this:
 *
 * 0x0000 - 0x07FF : 2KiB of RAM
 * 0x0800 - 0x0FFF : RAM mirrors
 * 0x1000 - 0x17FF : RAM mirrors
 * 0x1800 - 0x1FFF : RAM mirrors
 * 0x2000 - 0x2007 : PPU registers
 * 0x2008 - 0x3FFF : PPU registers mirrors
 * 0x4000 - 0x4017 : APU and IO registers
 * 0x4018 - 0x401F : disabled APU and IO functionality
 * 0x4020 - 0xFFFF : cartridge space
 */

class NES
{
public:
    NES();
    virtual ~NES() = default;

    // These are going to dispatch memory access
    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t data);

    void Tick();

    void ResetRAM();

    friend class Debugger;

private:
    // Peripherals attached to the NES
    CPU cpu;

    /*
     * 2KiB of main RAM available to the CPU,
     * the actual addressing space of the CPU
     * is of 64KiB, but most of it is just
     * mirrored or dedicated to IO registers.
     * Addresses from 0x0000 to 0x00FF is the
     * zero page, that can be accessed with a
     * 8 bit address, whereas memory locations
     * from 0x0100 to 0x01FF is the stack.
     */
    std::array<uint8_t, 2048> RAM;
};

#endif // NES_H
