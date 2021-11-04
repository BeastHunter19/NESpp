#ifndef BITMAPPEDREGISTER_H
#define BITMAPPEDREGISTER_H

#include <cstdint>

template <typename flagsEnum>
struct BitMappedRegister
{
    uint8_t value;

    template <flagsEnum flagMask>
    inline bool Test()
    {
        return value & flagMask;
    }

    template <flagsEnum flagMask>
    inline void Set()
    {
        value |= flagMask;
    }

    template <flagsEnum flagMask>
    inline void Toggle()
    {
        value ^= flagMask;
    }

    template <flagsEnum flagMask>
    inline void Clear()
    {
        value &= (~flagMask);
    }
};

#endif // BITMAPPEDREGISTER_H
