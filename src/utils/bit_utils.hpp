#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include <cstddef>

class BitUtils
{
public:
    BitUtils() = delete;

    static void setBit(char *bits, size_t pos);
    static bool isBitSet(char bits, size_t pos);
};

#endif // BIT_UTILS_HPP
