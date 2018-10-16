#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace split_index
{

namespace utils
{

struct MathUtils
{
    MathUtils() = delete;

    template<typename T>
    static std::vector<std::vector<T>> calcCartesianProduct(const std::vector<T> &vec, int repeat)
    {
        assert(repeat >= 2); // N-fold Cartesian product.

        std::vector<std::vector<T>> ret;

        if (vec.empty())
        {
            return ret;
        }

        std::vector<int> curI(repeat, 0);
        bool finished = false;

        while (true)
        {
            std::vector<int> curComb;

            for (const int i : curI)
            {
                curComb.push_back(vec[i]);
            }

            ret.push_back(curComb);
            int i = 0;

            while (curI[i] == static_cast<int>(vec.size()) - 1)
            {
                i += 1;

                if (i == curI.size())
                {
                    finished = true;
                    break;
                }
            }

            if (finished)
            {
                break;
            }

            curI[i] += 1;
            i -= 1;

            while (i >= 0)
            {
                curI[i] = 0;
                i -= 1;
            }
        }

        assert(ret.size() == std::pow(vec.size(), repeat));
        return ret;
    }
};

} // namespace utils

} // namespace split_index

#endif // MATH_UTILS_HPP
