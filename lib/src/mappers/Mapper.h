#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>

class Mapper
{
public:
    Mapper(int banksPRG, int banksCHR);
    virtual ~Mapper() = default;

    virtual uint16_t GetAddressPRG(uint16_t address) = 0;
    virtual uint16_t GetAddressCHR(uint16_t address) = 0;

protected:
    int banksPRG, banksCHR;
};

#endif // MAPPER_H
