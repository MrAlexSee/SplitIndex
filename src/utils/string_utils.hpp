#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <vector>

namespace split_index
{

namespace utils
{

struct StringUtils
{
    StringUtils() = delete;

    static std::string getElapsedInfo(long long elapsedUs, int nIter, int nQueries);
    static void printProgress(std::string info, int count, int size);
    
    /** Leaves only words having >= [minWordLength] characters in the [words] vector. */
    static void filterWordsByMinLength(std::vector<std::string> &words, int minWordLength);

    template<typename T>
    static std::string vecToStr(const std::vector<T> &vec, const std::string &separator = ",")
    {
        std::string ret = "[";

        for (size_t i = 0; i < vec.size(); ++i)
        {
            ret += std::to_string(vec[i]);

            if (i != vec.size() - 1)
            {
                ret += separator;
            }
        }

        return ret + "]";
    }
};

} // namespace utils

} // namespace split_index

#endif // STRING_UTILS_HPP
