#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include <cstddef>

namespace split_index
{

namespace utils
{

class BitUtils
{
public:
    BitUtils() = delete;

    static bool isBitSet(char bits, size_t pos);
    static void setBit(char *bits, size_t pos);
};

} // namespace utils

} // namespace fingerprints

#endif // BIT_UTILS_HPP
