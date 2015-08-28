#include "dist.hpp"

////////////////////
// We don't use any assembly magic here because the compiler with -O3 wins anyway (tested).
////////////////////

int Dist::calcHamming(const char *str1, const char *str2, size_t length)
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

bool Dist::isExactMatch(const char *str1, const char *str2, size_t length)
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
