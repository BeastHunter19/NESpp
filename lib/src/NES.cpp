#include "NES.h"

NES::NES()
    : cpu(this)
{
    ResetRAM();
}

uint8_t NES::Read(uint16_t address) const
{
    switch (address)
    {
    case 0x0000 ... 0x07FF: {
        return RAM[address];
        break;
    }
    default: return 0x00;
    }
}

void NES::Write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x0000 ... 0x07FF: {
        RAM[address] = data;
        break;
    }
    default: break;
    }
}

void NES::Tick()
{
    // TODO: step all the peripherals
}

void NES::ResetRAM()
{
    RAM.fill(0xFF);
}