#ifndef SPLIT_INDEX_K_HPP
#define SPLIT_INDEX_K_HPP

#include "split_index.hpp"

#ifndef SPLIT_INDEX_K_WHITEBOX
#define SPLIT_INDEX_K_WHITEBOX
#endif

namespace split_index
{

/** Split index for any k = 1, 2, 3. */
template<int k>
class SplitIndexK : public SplitIndex
{
public:
    SplitIndexK(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor);
    ~SplitIndexK() override;

protected:
    // We have 2 bits (4 positions) per word.
    static constexpr const int maxK = 3;

    SPLIT_INDEX_K_WHITEBOX
};

template<int k>
SplitIndexK<k>::SplitIndexK(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor)
    :SplitIndex(wordSet)
{

}

template<int k>
SplitIndexK<k>::~SplitIndexK()
{
    
}

} // namespace split_index

#endif // SPLIT_INDEX_K_HPP
