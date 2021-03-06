#ifndef SPLIT_INDEX_FACTORY_HPP
#define SPLIT_INDEX_FACTORY_HPP

#include <stdexcept>
#include <string>
#include <unordered_set>

#include "../hash_function/hash_functions.hpp"

#include "split_index_1.hpp"
#include "split_index_1_comp.hpp"
#include "split_index_1_comp_triple.hpp"
#include "split_index_k.hpp"

namespace split_index
{

struct SplitIndexFactory
{
    enum class IndexType { K1, K1Comp, K1CompTriple, K2, K3 };

    inline static SplitIndex *initIndex(const std::unordered_set<std::string> &words, 
        hash_functions::HashFunctions::HashType hashType, 
        IndexType indexType,
        float maxLoadFactor);
};

SplitIndex *SplitIndexFactory::initIndex(const std::unordered_set<std::string> &words, 
    hash_functions::HashFunctions::HashType hashType, 
    IndexType indexType,
    float maxLoadFactor)
{
    SplitIndex *index;
    
    switch (indexType)
    {
        case IndexType::K1:
            index = new SplitIndex1(words, hashType, maxLoadFactor);
            break;
        case IndexType::K1Comp:
            index = new SplitIndex1Comp(words, hashType, maxLoadFactor);
            break;
        case IndexType::K1CompTriple:
            index = new SplitIndex1CompTriple(words, hashType, maxLoadFactor);
            break;
        case IndexType::K2:
            index = new SplitIndexK<2>(words, hashType, maxLoadFactor);
            break;
        case IndexType::K3:
            index = new SplitIndexK<3>(words, hashType, maxLoadFactor);
            break;
        default:
            throw std::invalid_argument("bad index type: " + std::to_string(static_cast<int>(indexType)));
    }

    index->construct();
    return index;
}

} // namespace split_index

#endif // SPLIT_INDEX_FACTORY_HPP
