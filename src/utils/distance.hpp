#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include <cstddef>

namespace split_index
{

namespace utils
{

struct Distance
{
    Distance() = delete;

    template<unsigned k>
    static bool isHammingAtMostK(const char *str1, const char *str2, size_t length)
    {
        // With compiler optimizations, this version is faster than any bitwise/avx/sse magic (tested).
        unsigned nErrors = 0;

        for (size_t i = 0; i < length; ++i)
        {
            if (str1[i] != str2[i])
            {
                if (++nErrors > k)
                {
                    return false;
                }
            }
        }

        return true;
    }

    static unsigned calcHamming(const char *str1, const char *str2, size_t length)
    {
        unsigned nErrors = 0;

        for (size_t i = 0; i < length; ++i)
        {
            if (str1[i] != str2[i])
            {
                nErrors += 1;
            }
        }

        return nErrors;
    }
};

} // namespace utils

} // namespace split_index

#endif // DISTANCE_HPP
