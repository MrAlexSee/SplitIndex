#ifndef HASH_FUNCTIONS_HPP
#define HASH_FUNCTIONS_HPP

#include <functional>

namespace split_index
{

namespace hash_functions
{

struct HashFunctions
{
    using HashFunctionType = size_t (*)(const char *, size_t length);
    HashFunctions() = delete;

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
