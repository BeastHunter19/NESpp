#include "MainBus.h"

MainBus::MainBus()
    : cpu(*this)
{
    ResetRAM();
}

uint8_t MainBus::Read(uint16_t address) const
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

void MainBus::Write(uint16_t address, uint8_t data)
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

void MainBus::Tick()
{
    // TODO: step all the peripherals
}

void MainBus::ResetRAM()
{
    RAM.fill(0xFF);
}