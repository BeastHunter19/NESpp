#ifndef BITMAPPEDREGISTER_H
#define BITMAPPEDREGISTER_H

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
