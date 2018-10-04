#include "distance.hpp"

namespace split_index
{

namespace utils
{

bool Distance::isExactMatch(const char *str1, const char *str2, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }

    return true;
}

int Distance::calcHamming(const char *str1, const char *str2, size_t length)
{
    int nErrors = 0;

    for (size_t i = 0; i < length; ++i)
    {
        if (str1[i] != str2[i])
        {
            ++nErrors;
        }
    }

    return nErrors;
}

} // namespace utils

} // namespace split_index
