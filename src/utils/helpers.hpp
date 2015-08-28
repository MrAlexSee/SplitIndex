#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <string>
#include <vector>

class Helpers
{
public:
    Helpers() = delete;

    static std::string getTimesStr(long long elapsedUs, int nQueries);
    static void printProgress(const std::string &info, int count, int size);

    static inline double round2Places(double d) { return round(d * 100.0) / 100.0; }

    template<typename T>
    static std::vector<std::vector<T>> countCartProd(const std::vector<T> &vec, int repeat)
    {
        assert(repeat >= 2); // N-fold Cartesian product.
        std::vector<std::vector<T>> res;

        std::vector<int> curI(repeat, 0);
        bool finished = false;

        while (true)
        {
            std::vector<int> curComb;

            for (const int i : curI)
            {
                curComb.push_back(vec[i]);
            }

            res.push_back(curComb);
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
                break;

            curI[i] += 1;
            i -= 1;

            while (i >= 0)
            {
                curI[i] = 0;
                i -= 1;
            }

        }

        assert(res.size() == std::pow(vec.size(), repeat));
        return res;
    }

    template<typename T>
    static std::string vecToStr(std::vector<T> vec)
    {
        std::string res = "[";

        for (size_t i = 0; i < vec.size(); ++i)
        {
            res += std::to_string(vec[i]);

            if (i != vec.size() - 1)
            {
                res += ", ";
            }
        }

        return res + "]";
    }
};

#endif // HELPERS_HPP
