#include <cassert>

#include "bit_utils.hpp"

using namespace std;

bool BitUtils::isBitSet(char bits, size_t pos)
{
    assert(pos >= 0 and pos < 8);
    unsigned char mask = 0x1u;

    mask <<= pos;
    return bits & mask;
}

void BitUtils::setBit(char *bits, size_t pos)
{
    assert(pos >= 0 and pos < 8);
    unsigned char mask = 0x1u;

    mask <<= pos;
    (*bits) |= mask;
}
