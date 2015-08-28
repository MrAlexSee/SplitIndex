#ifndef HASH_FUNCTIONS_HPP
#define HASH_FUNCTIONS_HPP

#include <cstddef>

class HashFunctions
{
public:
    typedef size_t (*HashFunctionType)(const char *, size_t length);

    HashFunctions() = delete;

    static size_t F_City     (const char *str, size_t length);
    static size_t F_FNV1     (const char *str, size_t length);
    static size_t F_FNV1a    (const char *str, size_t length);
    static size_t F_Murmur3  (const char *str, size_t length);
    static size_t F_sdbm     (const char *str, size_t length);
    static size_t F_SpookyV2 (const char *str, size_t length);
    static size_t F_SuperFast(const char *str, size_t length);
    static size_t F_xxHash   (const char *str, size_t length);
    static size_t F_farsh    (const char *str, size_t length);
    static size_t F_Farm     (const char *str, size_t length);
};

#endif // HASH_FUNCTIONS_HPP
