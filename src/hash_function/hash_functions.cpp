#include "hash_functions.hpp"

#include "city.h"
#include "farsh.h"
#include "farmhash.h"
#include "murmur3.h"
#include "spooky_v2.h"
#include "super_fast_hash.h"
#include "xxhash.h"

namespace split_index
{

namespace hash_functions
{

size_t HashFunctions::city(const char *str, size_t length)
{
    return CityHash64(str, length);
}

size_t HashFunctions::farm(const char *str, size_t length)
{
    return util::Hash32(str, length);
}

size_t HashFunctions::farsh(const char *str, size_t length)
{
    return farsh_hash::farsh(str, length);
}

size_t HashFunctions::fnv1(const char *str, size_t length)
{
    size_t hash = 0x811C9DC5;

    for (size_t i = 0; i < length; ++i)
    {
        hash *= 0x01000193;
        hash ^= str[i];
    }

    return hash;    
}

size_t HashFunctions::fnv1a(const char *str, size_t length)
{
    size_t hash = 0x811C9DC5;

    for (size_t i = 0; i < length; ++i)
    {
        hash ^= str[i];
        hash *= 0x01000193;
    }

    return hash;
}

size_t HashFunctions::murmur3(const char *str, size_t length)
{
    return MurmurHash3_x86_32(str, length, 0);
}

size_t HashFunctions::sdbm(const char *str, size_t length)
{   
    size_t hash = 0;
 
    for (size_t i = 0; i < length; ++i)
    {
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

size_t HashFunctions::spookyV2(const char *str, size_t length)
{
    return SpookyHash::Hash32(str, length, 0);
}

size_t HashFunctions::superFast(const char *str, size_t length)
{
    return SuperFastHash(reinterpret_cast<const signed char *>(str), length);
}

size_t HashFunctions::xxHash(const char *str, size_t length)
{
    return XXH32(str, length, 0);
}

} // namespace hash_functions

} // namespace split_index
