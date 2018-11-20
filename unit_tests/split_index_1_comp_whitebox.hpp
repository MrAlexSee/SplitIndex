#ifndef SPLIT_INDEX_1_COMP_WHITEBOX_HPP
#define SPLIT_INDEX_1_COMP_WHITEBOX_HPP

#ifndef SPLIT_INDEX_1_COMP_WHITEBOX
#define SPLIT_INDEX_1_COMP_WHITEBOX \
    friend struct SplitIndex1CompWhitebox;
#endif

#include "../src/index/split_index_1_comp.hpp"

namespace split_index
{

struct SplitIndex1CompWhitebox
{
    SplitIndex1CompWhitebox() = delete;

    inline static void fillQgramMaps(SplitIndex1Comp &index)
    {
        index.fillQgramMaps();
    }

    inline static std::vector<std::string> calcQGramsOrderedByFrequency(const SplitIndex1Comp &index)
    {
        return index.calcQGramsOrderedByFrequency();
    }

    inline static size_t encodeToBuf(SplitIndex1Comp &index, const char *word, size_t wordSize)
    {
        return index.encodeToBuf(word, wordSize);
    }

    inline static size_t decodeToBuf(SplitIndex1Comp &index, const char *word, size_t wordSize, size_t maxDecodedWordSize)
    {
        return index.decodeToBuf(word, wordSize, maxDecodedWordSize);
    }

    inline static const char *getCodingBuf(const SplitIndex1Comp &index)
    {
        return index.codingBuf;
    }

    inline static const std::map<std::string, char> getQGramToCharMap(const SplitIndex1Comp &index)
    {
        return index.qgramToChar;        
    }

    inline static const std::map<char, std::string> getCharToQgramMap(const SplitIndex1Comp &index)
    {
        return index.charToQgram;
    }
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_WHITEBOX_HPP
