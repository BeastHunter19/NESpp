#ifndef BITMAPPEDREGISTER_H
#define BITMAPPEDREGISTER_H

#include <bit>
#include <cstdint>

template <typename FlagsEnum>
struct BitMappedRegister
{
    uint8_t value;

    template <FlagsEnum flagMask>
    inline int Test()
    {
        if ((value & flagMask) == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    template <FlagsEnum flagMask>
    inline void Assign(uint8_t val)
    {
        int leftZeros = std::countr_zero((uint8_t)flagMask);
        uint8_t actualMask = val << leftZeros;
        value &= (~flagMask);
        value |= actualMask;
    }

    template <FlagsEnum flagMask>
    inline void Set()
    {
        value |= flagMask;
    }

    template <FlagsEnum flagMask>
    inline void Toggle()
    {
        value ^= flagMask;
    }

    template <FlagsEnum flagMask>
    inline void Clear()
    {
        value &= (~flagMask);
    }
};

#endif // BITMAPPEDREGISTER_H
