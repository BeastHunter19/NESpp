#include "MainBus.h"

MainBus::MainBus(CPU* cpu)
    : cpu(cpu)
{
    this->cpu->AttachToBus(this);
}

void MainBus::ResetRAM()
{
    RAM.fill(0xFF);
}
