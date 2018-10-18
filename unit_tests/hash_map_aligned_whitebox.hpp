#ifndef HASH_MAP_ALIGNED_WHITEBOX_HPP
#define HASH_MAP_ALIGNED_WHITEBOX_HPP

#ifndef HASH_MAP_ALIGNED_WHITEBOX
#define HASH_MAP_ALIGNED_WHITEBOX \
    friend struct HashMapAlignedWhitebox;
#endif

#include "../src/hash_map/hash_map_aligned.hpp"

namespace split_index
{

namespace hash_map
{

struct HashMapAlignedWhitebox
{
public:
    HashMapAlignedWhitebox() = delete;

    inline static char *createBucket(const hash_map::HashMapAligned &hashMap, const char *key, size_t keySize, char *entry)
    {
        return hashMap.createBucket(key, keySize, entry);
    }
};

} // namespace hash_map

} // namespace split_index

#endif // HASH_MAP_ALIGNED_WHITEBOX_HPP
