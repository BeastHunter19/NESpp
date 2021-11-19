#ifndef NROM_H
#define NROM_H

#include "Mapper.h"

class NROM : public Mapper
{
public:
    NROM(int banksPRG, int banksCHR);
    ~NROM() = default;

    uint16_t GetAddressPRG(uint16_t address) override;
    uint16_t GetAddressCHR(uint16_t address) override;
};

#endif // NROM_H
