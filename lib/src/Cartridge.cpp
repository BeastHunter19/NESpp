#include <fstream>
#include "Cartridge.h"
#include "mappers/NROM.h"

Cartridge::Cartridge()
    : PRG_ROM(32768, 0x00), CHR_ROM(8192, 0x00)
{
    validRom = false;
    banksPRG = 1;
    banksCHR = 1;
    mapper = std::make_unique<NROM>(banksPRG, banksCHR);
}

void Cartridge::LoadFile(const std::filesystem::path& pathToROM)
{
    std::ifstream rom(pathToROM, std::ios::binary);
    rom.read(reinterpret_cast<char*>(&header), 16);
    if(header.constants[0] != 0x4E || header.constants[1] != 0x45 ||
       header.constants[2] != 0x53 || header.constants[3] != 0x1A)
    {
        return;
    }
    banksPRG = header.sizePRG;
    PRG_ROM.resize(16384 * banksPRG);
    banksCHR = header.sizeCHR;
    CHR_ROM.resize(8192 * banksCHR);

    if(header.flags6 & 0x08)
    {
        mirroring = FOUR_SCREEN;
    }
    else if(header.flags6 & 0x01)
    {
        mirroring = VERTICAL;
    }
    else
    {
        mirroring = HORIZONTAL;
    }

    uint8_t mapperNumber = (header.flags7 & 0xF0) | ((header.flags6 & 0xF0) >> 4);

    // Identify mapper type
    switch(mapperNumber)
    {
    case 0x00: break; // Mapper is already NROM by default
    }

    if(header.flags6 & 0x04)
    {
        rom.ignore(512);
    }
    rom.read(reinterpret_cast<char*>(PRG_ROM.data()), 16384 * banksPRG);
    rom.read(reinterpret_cast<char*>(CHR_ROM.data()), 8192 * banksCHR);
    validRom = true;
}

uint8_t Cartridge::ReadFromPRG(uint16_t address) const
{
    return PRG_ROM[mapper->GetAddressPRG(address)];
}

uint8_t Cartridge::ReadFromCHR(uint16_t address) const
{
    return CHR_ROM[mapper->GetAddressCHR(address)];
}

bool Cartridge::IsValid() const
{
    return validRom;
}
