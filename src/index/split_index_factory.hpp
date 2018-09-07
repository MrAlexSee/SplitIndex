#ifndef SPLIT_INDEX_FACTORY_HPP
#define SPLIT_INDEX_FACTORY_HPP

#include <string>
#include <vector>

#include "split_index_1.hpp"

namespace split_index
{

struct SplitIndexFactory
{
public:
    inline static SplitIndex *initIndex(const std::vector<std::string> &words, int minWordLength, int indexType);
};

SplitIndex *SplitIndexFactory::initIndex(const std::vector<std::string> &words, int minWordLength, int indexType)
{
    SplitIndex *index = new SplitIndex1(words, minWordLength);
    index->construct();

    return index;
}

} // namespace split_index

#endif // SPLIT_INDEX_FACTORY_HPP
