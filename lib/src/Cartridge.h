#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <vector>
#include <memory>
#include <filesystem>
#include "mappers/Mapper.h"

class Cartridge
{
public:
    Cartridge();
    ~Cartridge() = default;

    void LoadFile(const std::filesystem::path& pathToROM);

    uint8_t ReadFromPRG(uint16_t address) const;
    uint8_t ReadFromCHR(uint16_t address) const;

    bool IsValid() const;

    friend class Debugger;
private:
    bool validRom;
    int banksPRG, banksCHR;
    std::vector<uint8_t> PRG_ROM;
    std::vector<uint8_t> CHR_ROM;

    enum NametableMirroring
    {
        HORIZONTAL,
        VERTICAL,
        SINGLE_SCREEN,
        FOUR_SCREEN
    } mirroring;

    struct iNES_HeaderFormat
    {
        uint8_t constants[4];
        uint8_t sizePRG;
        uint8_t sizeCHR;
        uint8_t flags6;
        uint8_t flags7;
        uint8_t flags8;
        uint8_t flags9;
        uint8_t flags10;
        uint8_t padding[5];
    } header;

    std::unique_ptr<Mapper> mapper;
};

#endif // CARTRIDGE_H
