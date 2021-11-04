#include "CPU.h"

CPU::CPU() {}

void CPU::AttachToBus(MainBus* mainBus)
{
    this->mainBus = mainBus;
}

void CPU::Run() {}
