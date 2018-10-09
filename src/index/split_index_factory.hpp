#ifndef SPLIT_INDEX_FACTORY_HPP
#define SPLIT_INDEX_FACTORY_HPP

#include <stdexcept>
#include <string>
#include <unordered_set>

#include "../hash_function/hash_functions.hpp"
#include "split_index_1.hpp"

namespace split_index
{

struct SplitIndexFactory
{
    enum class IndexType { K1, K1Comp };

    inline static SplitIndex *initIndex(const std::unordered_set<std::string> &words, 
        hash_functions::HashFunctions::HashType hashType, 
        IndexType indexType);
};

SplitIndex *SplitIndexFactory::initIndex(const std::unordered_set<std::string> &words, 
    hash_functions::HashFunctions::HashType hashType, 
    IndexType indexType)
{
    SplitIndex *index;
    
    switch (indexType)
    {
        case IndexType::K1:
            index = new SplitIndex1(words, hashType);
            break;
        // case IndexType::K1Comp:
            // index = new SplitIndex1Comp(words, hashType);
            // break;
        default:
            throw std::invalid_argument("bad index type: " + std::to_string(static_cast<int>(indexType)));
    }

    index->construct();
    return index;
}

} // namespace split_index

#endif // SPLIT_INDEX_FACTORY_HPP
