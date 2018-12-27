#ifndef SPLIT_INDEX_1_WHITEBOX_HPP
#define SPLIT_INDEX_1_WHITEBOX_HPP

#ifndef SPLIT_INDEX_1_WHITEBOX
#define SPLIT_INDEX_1_WHITEBOX \
    friend struct SplitIndex1Whitebox;
#endif

#include "../src/index/split_index_1.hpp"

namespace split_index
{

struct SplitIndex1Whitebox
{
    SplitIndex1Whitebox() = delete;

    inline static size_t getPrefixSize(const SplitIndex1 &index)
    {
        return index.prefixSize;
    }

    inline static char *getPrefixBuf(const SplitIndex1 &index)
    {
        return index.prefixBuf;
    }

    inline static size_t getSuffixSize(const SplitIndex1 &index)
    {
        return index.suffixSize;
    }

    inline static char *getSuffixBuf(const SplitIndex1 &index)
    {
        return index.suffixBuf;
    }

    inline static size_t calcEntrySizeB(const SplitIndex1 &index, const char *entry)
    {
        return index.calcEntrySizeB(entry);
    }

    inline static int calcEntryNWords(const char *entry)
    {
        return SplitIndex1::calcEntryNWords(entry);
    }

    inline static void storePrefixSuffixInBuffers(SplitIndex1 &index, const std::string &word)
    {
        index.storePrefixSuffixInBuffers(word);
    }

    inline static char *createEntry(const SplitIndex1 &index, const char *wordPart, size_t partSize, bool isPartSuffix)
    {
        return index.createEntry(wordPart, partSize, isPartSuffix);
    }

    inline static void addToEntry(const SplitIndex1 &index, char **entryPtr, const char *wordPart, size_t partSize, bool isPartSuffix)
    {
        return index.addToEntry(entryPtr, wordPart, partSize, isPartSuffix);
    }

    inline static char *advanceInEntryByWordCount(const SplitIndex1 &index, char *entry, uint16_t nWords)
    {
        return index.advanceInEntryByWordCount(entry, nWords);
    }
};

} // namespace split_index

#endif // SPLIT_INDEX_1_WHITEBOX_HPP
