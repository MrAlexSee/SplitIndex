#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include <cstddef>

namespace split_index
{

namespace utils
{

struct BitUtils
{
    BitUtils() = delete;

    static bool isBitSet(char bits, size_t pos);
    static void setBit(char *bits, size_t pos);
};

} // namespace utils

} // namespace split_index

#endif // BIT_UTILS_HPP
