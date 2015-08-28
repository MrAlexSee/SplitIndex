#include "hash_functions.hpp"

using namespace std;

#include "city.h"
#include "farsh.h"
#include "farmhash.h"
#include "murmur3.h"
#include "spookyV2.h"
#include "super_fast_hash.h"
#include "xxhash.h"

size_t HashFunctions::F_City(const char *str, size_t length)
{
    return CityHash64(str, length);
}

size_t HashFunctions::F_Farm(const char *str, size_t length)
{
    return NAMESPACE_FOR_HASH_FUNCTIONS::Hash32(str, length);
}

size_t HashFunctions::F_FNV1(const char *str, size_t length)
{
    size_t hash = 0x811C9DC5;

    for (size_t i = 0; i < length; ++i)
    {
        hash *= 0x01000193;
        hash ^= str[i];
    }

    return hash;    
}

size_t HashFunctions::F_FNV1a(const char *str, size_t length)
{
    size_t hash = 0x811C9DC5;

    for (size_t i = 0; i < length; ++i)
    {
        hash ^= str[i];
        hash *= 0x01000193;
    }

    return hash;
}

size_t HashFunctions::F_Murmur3(const char *str, size_t length)
{
    return MurmurHash3_x86_32(str, length, 0);
}

size_t HashFunctions::F_sdbm(const char *str, size_t length)
{   
    size_t hash = 0;
 
    for (size_t i = 0; i < length; ++i)
    {
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

size_t HashFunctions::F_SpookyV2(const char *str, size_t length)
{
    return SpookyHash::Hash32(str, length, 0);
}

size_t HashFunctions::F_SuperFast(const char *str, size_t length)
{
    return SuperFastHash(reinterpret_cast<const signed char *>(str), length);
}

size_t HashFunctions::F_xxHash(const char *str, size_t length)
{
    return XXH32(str, length, 0);
}

size_t HashFunctions::F_farsh(const char *str, size_t length)
{
    return farsh(str, length);
}
