#ifndef SPLIT_INDEX_1_COMP_TRIPLE_HPP
#define SPLIT_INDEX_1_COMP_TRIPLE_HPP

#include "split_index_1_comp.hpp"

#ifndef SPLIT_INDEX_1_COMP_WHITEBOX
#define SPLIT_INDEX_1_COMP_WHITEBOX
#endif

namespace split_index
{

/** Split index for k = 1, using compression with a combination of 2-, 3-, and 4-grams. */
class SplitIndex1CompTriple : public SplitIndex1Comp
{
public:
    SplitIndex1CompTriple(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType,
        float maxLoadFactor);
    ~SplitIndex1CompTriple();

    std::string toString() const override;

protected:
    void calcQgramsAndFillMaps() override;

    size_t encodeToBuf(const char *word, size_t wordSize) override;
    size_t encodeToBuf(char *dstBuf, const char *word, size_t curWordSize, size_t curQgramSize);

    char *encodingTmpBuf1 = nullptr;
    char *encodingTmpBuf2 = nullptr;

    /** These are actual q-gram counts extracted from the text.
     * Cannot be greater than the constants below. */
    size_t curN2grams = 0, curN3grams = 0, curN4grams = 0;

    /** These are all compile-time constants, determined earlier. */
    static constexpr size_t n2grams = 96, n3grams = 3, n4grams = 1;

    // We can take advantage of the whitebox class for the base class since functions are overridden here.
    SPLIT_INDEX_1_COMP_WHITEBOX
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_TRIPLE_HPP

