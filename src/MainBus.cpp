#include "MainBus.h"

MainBus::MainBus(CPU* cpu)
    : cpu(cpu)
{
    this->cpu->AttachToBus(this);
}
