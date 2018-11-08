#ifndef SPLIT_INDEX_1_COMP_HPP
#define SPLIT_INDEX_1_COMP_HPP

#include <map>

#include "split_index_1.hpp"

#ifndef SPLIT_INDEX_1_COMP_WHITEBOX
#define SPLIT_INDEX_1_COMP_WHITEBOX
#endif

namespace split_index
{

/** Split index for k = 1 using compression. */
class SplitIndex1Comp : public SplitIndex1
{
public:
    SplitIndex1Comp(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor);
    ~SplitIndex1Comp();

    void construct() override;
    std::string toString() const override;

protected:
    void fillQgramMaps();
    std::vector<std::string> calcQGramsOrderedByFrequency() const;

    void initEntry(const std::string &word) override;

    /** Encodes [word] of size [wordSize] into codingBuf. Returns the size of encoded word. */
    virtual size_t encodeToBuf(const char *word, size_t wordSize);
    /** Decodes [word] of size [wordSize] into codingBuf. 
     * Returns the size of decoded word or 0 if [maxDecodedWordSize] is exceeded. */
    virtual size_t decodeToBuf(const char *word, size_t wordSize, size_t maxDecodedWordSize);

    char *codingBuf = nullptr;

    /** A map q-gram -> char. */
    std::map<std::string, char> qgramToChar;
    /** A map char -> q-gram. */
    std::map<char, std::string> charToQgram;

    /** Total number of q-grams used for encoding symbols. */
    const size_t nQgrams = 100;
    /** Size of each q-gram, e.g., use 2 for digrams. */
    const size_t qgramSize = 2;

    /** First char value which is used for q-gram encoding (the rest is consecutive).
     * This assumes that the code is compiled using unsigned chars. */
    static constexpr char firstChar = 128;

    SPLIT_INDEX_1_COMP_WHITEBOX
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_HPP
