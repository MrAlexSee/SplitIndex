#ifndef DIST_HPP
#define DIST_HPP

#include <cstddef>

class Dist
{
public:
    Dist() = delete;

    static bool isExactMatch(const char *str1, const char *str2, size_t length);

    static int calcHamming(const char *str1, const char *str2, size_t length);

    template<int k> // #errors <= k
    static bool isHammingK(const char *str1, const char *str2, size_t length)
    {
        int nErrors = 0;

        for (size_t i = 0; i < length; ++i)
        {
            if (str1[i] != str2[i])
            {
                if (++nErrors > k)
                    return false;
            }
        }

        return true;
    }
};

#endif // DIST_HPP
