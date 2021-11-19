#include "NES.h"
#include <filesystem>

NES::NES()
    : cpu(*this)
{
    ResetRAM();
}

uint8_t NES::Read(uint16_t address) const
{
    switch (address)
    {
    case 0x0000 ... 0x1FFF: {
        return RAM[address % 0x0800];
        break;
    }
    case 0x2000 ... 0x3FFF: // PPU
    case 0x4000 ... 0x4017: // APU and IO
    case 0x4018 ... 0x401F: // disabled
    case 0x4020 ... 0xFFFF: {
        return cart.ReadFromPRG(address - 0x8000);
        break;
    }
    default: return 0x00;
    }
}

void NES::Write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x0000 ... 0x1FFF: {
        RAM[address % 0x0800] = data;
        break;
    }
    case 0x2000 ... 0x3FFF: // PPU
    case 0x4000 ... 0x4017: // APU and IO
    case 0x4018 ... 0x401F: // disabled
    case 0x4020 ... 0xFFFF: // Cartridge
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

bool NES::LoadGame(const std::string& pathToROM)
{
    std::filesystem::path path(pathToROM);
    if(!std::filesystem::exists(path)){
        return false;
    }
    cart.LoadFile(path);
    if(!cart.IsValid())
    {
        return false;
    }
    cpu.Reset();
    return true;
}
