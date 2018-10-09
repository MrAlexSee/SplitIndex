#ifndef HASH_FUNCTIONS_HPP
#define HASH_FUNCTIONS_HPP

#include <functional>
#include <stdexcept>

namespace split_index
{

namespace hash_functions
{

struct HashFunctions
{
    enum class HashType { City, Farm, Farsh, FNV1, FNV1a, Murmur3, Sdbm, SpookyV2, SuperFast, XxHash };

    using HashFunctionType = size_t (*)(const char *, size_t length);
    HashFunctions() = delete;

    static HashFunctionType getHashFunction(HashType hashType)
    {
        switch (hashType)
        {
            case HashType::City:
                return &city;
            case HashType::Farm:
                return &farm;
            case HashType::Farsh:
                return &farsh;
            case HashType::FNV1:
                return &fnv1;
            case HashType::FNV1a:
                return &fnv1a;
            case HashType::Murmur3:
                return &murmur3;
            case HashType::Sdbm:
                return &sdbm;
            case HashType::SpookyV2:
                return &spookyV2;
            case HashType::SuperFast:
                return &superFast;
            case HashType::XxHash:
                return &xxHash;
            default:
                throw std::invalid_argument("bad hash type");
        }
    }

    static size_t city     (const char *str, size_t length);
    static size_t farm     (const char *str, size_t length);
    static size_t farsh    (const char *str, size_t length);
    static size_t fnv1     (const char *str, size_t length);
    static size_t fnv1a    (const char *str, size_t length);
    static size_t murmur3  (const char *str, size_t length);
    static size_t sdbm     (const char *str, size_t length);
    static size_t spookyV2 (const char *str, size_t length);
    static size_t superFast(const char *str, size_t length);
    static size_t xxHash   (const char *str, size_t length);
};

} // namespace hash_functions

} // namespace split_index

#endif // HASH_FUNCTIONS_HPP
