#include "MainBus.h"

MainBus::MainBus(CPU* cpu)
    : cpu(cpu)
{
    this->cpu->AttachToBus(this);
    ResetRAM();
}

uint8_t MainBus::Read(uint16_t address) const
{
    return uint8_t{};
}

void MainBus::Write(uint16_t address, uint8_t data) {}

void MainBus::ResetRAM()
{
    RAM.fill(0xFF);
}
