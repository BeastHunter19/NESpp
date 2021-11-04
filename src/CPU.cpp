#include "CPU.h"
#include "MainBus.h"

CPU::CPU()
{
    PS.value = 0x34;
    A = X = Y = 0;
    SP = 0xFD;
    mainBus->ResetRAM();

    // TODO: revisit these
    Write(0x4017, 0x00);
    Write(0x4015, 0x00);
    for (uint16_t lastBits = 0x0000; lastBits <= 0x0013; lastBits++)
    {
        uint16_t address = 0x4000 & lastBits;
        mainBus->Write(address, 0x00);
    }

    // TODO: initialize APU registers
}

void CPU::AttachToBus(MainBus* mainBus)
{
    this->mainBus = mainBus;
}

uint8_t CPU::Read(uint16_t address)
{
    return mainBus->Read(address);
}

void CPU::Write(uint16_t address, uint8_t data)
{
    mainBus->Write(address, data);
}

void CPU::LoadInstrFromString(const std::string& instructions) {}

void CPU::Reset()
{
    SP -= 3;
    PS.Set<I>();

    // TODO: reset APU status
}

void CPU::Run() {}
