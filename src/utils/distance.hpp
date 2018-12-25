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
};

} // namespace utils

} // namespace split_index

#endif // DISTANCE_HPP
