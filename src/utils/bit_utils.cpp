#include <cassert>

#include "bit_utils.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

void BitUtils::setBit(char *bits, size_t pos)
{
    assert(pos >= 0 and pos < 8);
    const unsigned char mask = 0x1u;

    (*bits) |= (mask << pos);
}

bool BitUtils::isBitSet(char bits, size_t pos)
{
    assert(pos >= 0 and pos < 8);
    const unsigned char mask = 0x1u;

    return bits & (mask << pos);
}

} // namespace utils

} // namespace fingerprints
