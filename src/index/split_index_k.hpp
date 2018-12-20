#ifndef SPLIT_INDEX_K_HPP
#define SPLIT_INDEX_K_HPP

#include <stdexcept>

#include "split_index.hpp"

#include "../hash_map/hash_map_aligned.hpp"

#ifndef SPLIT_INDEX_K_WHITEBOX
#define SPLIT_INDEX_K_WHITEBOX
#endif

namespace split_index
{

/** Split index for any k = 1, 2, 3. */
template<size_t k>
class SplitIndexK : public SplitIndex
{
public:
    SplitIndexK(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor);
    ~SplitIndexK() override;

protected:
    void initEntry(const std::string &word) override;
    void processQuery(const std::string &query, std::set<std::string> &results) override;

    size_t calcEntrySizeB(const char *entry) const override;

    // We have 2 bits (4 positions) per word.
    static constexpr const size_t maxK = 3;

    char *parts[k + 1];
    size_t partSizes[k + 1];

    SPLIT_INDEX_K_WHITEBOX
};

template<size_t k>
SplitIndexK<k>::SplitIndexK(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor)
    :SplitIndex(wordSet)
{
    if (k < 1 or k > maxK)
    {
        throw std::invalid_argument("k must be between 1 and maxK");
    }

    auto calcEntrySizeB = std::bind(&SplitIndexK<k>::calcEntrySizeB, this, std::placeholders::_1);
    const int nBucketsHint = std::max(1, static_cast<int>(nBucketsHintFactor * wordSet.size()));

    hashMap = new hash_map::HashMapAligned(calcEntrySizeB, maxLoadFactor, nBucketsHint, hashType);

    for (size_t i = 0; i < k + 1; ++i)
    {
        parts[i] = new char[maxWordSize];
    }
}

template<size_t k>
SplitIndexK<k>::~SplitIndexK()
{
    for (size_t i = 0; i < k + 1; ++i)
    {
        delete[] parts[i];
    }
}

template<size_t k>
void SplitIndexK<k>::initEntry(const std::string &word)
{

}

template<size_t k>
void SplitIndexK<k>::processQuery(const std::string &query, std::set<std::string> &results)
{

}

template<size_t k>
size_t SplitIndexK<k>::calcEntrySizeB(const char *entry) const
{

}

} // namespace split_index

#endif // SPLIT_INDEX_K_HPP
