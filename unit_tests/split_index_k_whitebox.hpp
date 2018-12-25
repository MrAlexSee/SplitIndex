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
    inline static size_t calcEntrySizeB(const SplitIndexK<k> &index, const char *entry)
    {
        return index.calcEntrySizeB(entry);
    }

    template<size_t k>
    inline static char *createEntry(const char *wordParts, size_t partsSize, size_t iPart)
    {
        return SplitIndexK<k>::createEntry(wordParts, partsSize, iPart);
    }

    template<size_t k>
    inline static void addToEntry(const SplitIndexK<k> &index, char **entryPtr ,const char *wordParts, size_t partsSize, size_t iPart)
    {
        return index.addToEntry(entryPtr, wordParts, partsSize, iPart);
    }


    template<size_t k>
    inline static size_t getPartSize(size_t wordSize)
    {
        return SplitIndexK<k>::getPartSize(wordSize);
    }
};

} // namespace split_index

#endif // SPLIT_INDEX_K_WHITEBOX_HPP
