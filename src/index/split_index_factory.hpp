#ifndef SPLIT_INDEX_FACTORY_HPP
#define SPLIT_INDEX_FACTORY_HPP

#include <string>
#include <vector>

#include "split_index_1.hpp"
#include "split_index_1_comp.hpp"

namespace split_index
{

struct SplitIndexFactory
{
    enum class HashType { City, Farm, Farsh, FNV1, FNV1a, Murmur3, Sdbm, SpookyV2, SuperFast, XxHash };
    enum class IndexType { K1, K1Comp };

public:
    inline static SplitIndex *initIndex(const std::vector<std::string> &words, HashType hashType, IndexType indexType, int minWordLength);
};

SplitIndex *SplitIndexFactory::initIndex(const std::vector<std::string> &words, HashType hashType, IndexType indexType, int minWordLength)
{
    SplitIndex *index;
    
    switch (indexType)
    {
        case IndexType::K1:
            index = new SplitIndex1(words, minWordLength);
            break;
        case IndexType::K1Comp:
            index = new SplitIndex1Comp(words, minWordLength);
        default:
            assert(false);
            return nullptr;
    }

    index->construct();
    return index;
}

} // namespace split_index

#endif // SPLIT_INDEX_FACTORY_HPP
