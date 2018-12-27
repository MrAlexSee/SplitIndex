#ifndef SPLIT_INDEX_K_WHITEBOX_HPP
#define SPLIT_INDEX_K_WHITEBOX_HPP

#ifndef SPLIT_INDEX_K_WHITEBOX
#define SPLIT_INDEX_K_WHITEBOX \
    friend struct SplitIndexKWhitebox;
#endif

#include "../src/index/split_index_k.hpp"

namespace split_index
{

struct SplitIndexKWhitebox
{
    SplitIndexKWhitebox() = delete;

    template<size_t k>
    inline static const char *const *getWordPartBuf(const SplitIndexK<k> &index)
    {
        return index.wordPartBuf;
    }

    template<size_t k>
    inline static const size_t *getWordPartSizes(const SplitIndexK<k> &index)
    {
        return index.wordPartSizes;
    }

    template<size_t k>
    inline static size_t calcEntrySizeB(const SplitIndexK<k> &index, const char *entry)
    {
        return index.calcEntrySizeB(entry);
    }

    template<size_t k>
    inline static size_t calcEntryNWords(const char *entry)
    {
        return SplitIndexK<k>::calcEntryNWords(entry);
    }

    template<size_t k>
    inline static void storeWordPartsInBuffers(SplitIndexK<k> &index, const std::string &word)
    {
        index.storeWordPartsInBuffers(word);
    }

    template<size_t k>
    inline static size_t getPartSize(size_t wordSize)
    {
        return SplitIndexK<k>::getPartSize(wordSize);
    }

    template<size_t k>
    inline static char *createEntry(const char *wordParts, size_t partsSize, size_t iPart)
    {
        return SplitIndexK<k>::createEntry(wordParts, partsSize, iPart);
    }

    template<size_t k>
    inline static void addToEntry(const SplitIndexK<k> &index,
        char **entryPtr, const char *wordParts,
        size_t partsSize, size_t iPart)
    {
        return index.addToEntry(entryPtr, wordParts, partsSize, iPart);
    }

    template<size_t k>
    inline static std::string tryMatchPart(const SplitIndexK<k> &index,
        const std::string &query, const char *entry,
        size_t matchSize, size_t iPart)
    {
        return index.tryMatchPart(query, entry, matchSize, iPart);
    }

    template<size_t k>
    static void setPartBits(char *entry, size_t iWord, size_t iPart)
    {
        SplitIndexK<k>::setPartBits(entry, iWord, iPart);
    }

    template<size_t k>
    static size_t retrievePartIndexFromBits(const char *entry, size_t iWord)
    {
        return SplitIndexK<k>::retrievePartIndexFromBits(entry, iWord);
    }
};

} // namespace split_index

#endif // SPLIT_INDEX_K_WHITEBOX_HPP
