#ifndef SPLIT_INDEX_FACTORY_HPP
#define SPLIT_INDEX_FACTORY_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "split_index_1.hpp"

namespace split_index
{

struct SplitIndexFactory
{
public:
    inline static SplitIndex *initIndex(const std::vector<std::string> &words, int indexType, int minWordLength);
};

SplitIndex *SplitIndexFactory::initIndex(const std::vector<std::string> &words, int indexType, int minWordLength)
{
    SplitIndex *index;
    
    switch (indexType)
    {
        case 0:
            index = new SplitIndex1(words, minWordLength);
            break;
        default:
            throw std::invalid_argument("bad index type: " + std::to_string(indexType));
    }

    index->construct();
    return index;
}

} // namespace split_index

#endif // SPLIT_INDEX_FACTORY_HPP
