#ifndef QUERY_UTILS_HPP
#define QUERY_UTILS_HPP

#include <string>
#include <vector>

namespace split_index
{

namespace utils
{

class QueryUtils
{
public:
    QueryUtils() = delete;
    
    static std::vector<std::string> genQueries(const std::vector<std::string> &words,
                                               const std::string &alphabet,
                                               int minWordLength, int nQueries, int maxNErrors);
};

} // namespace utils

} // namespace split_index

#endif // QUERY_UTILS_HPP
