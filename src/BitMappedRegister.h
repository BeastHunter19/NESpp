#ifndef BITMAPPEDREGISTER_H
#define BITMAPPEDREGISTER_H

#include <cstdint>

template <typename FlagsEnum>
struct BitMappedRegister
{
    uint8_t value;

    template <FlagsEnum flagMask>
    inline bool Test()
    {
        return value & flagMask;
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
