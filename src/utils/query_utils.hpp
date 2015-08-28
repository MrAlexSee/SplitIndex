#ifndef GEN_QUERIES_HPP
#define GEN_QUERIES_HPP

#include <string>
#include <vector>

class QueryUtils
{
public:
    QueryUtils() = delete;
    
    static std::vector<std::string> genQueries(const std::vector<std::string> &words,
                                               const std::string &alphabet,
                                               int minWordLength, int nQueries, int maxNErrors);
};

#endif // GEN_QUERIES_HPP
