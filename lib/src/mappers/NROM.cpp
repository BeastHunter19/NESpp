#include "NROM.h"

NROM::NROM(int banksPRG, int banksCHR)
    : Mapper(banksPRG, banksCHR)
{}

uint16_t NROM::GetAddressPRG(uint16_t address)
{
    return address % (16384 * banksPRG);
}

uint16_t NROM::GetAddressCHR(uint16_t address)
{
    return address % 8192;
}
