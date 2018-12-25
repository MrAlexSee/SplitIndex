#ifndef SPLIT_INDEX_K_HPP
#define SPLIT_INDEX_K_HPP

#include <cmath>
#include <cstring>
#include <stdexcept>

#include "split_index.hpp"

#include "../hash_map/hash_map_aligned.hpp"
#include "../utils/distance.hpp"

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

    std::string toString() const override;

protected:
    void initEntry(const std::string &word) override;
    void processQuery(const std::string &query, std::set<std::string> &results) override;

    size_t calcEntrySizeB(const char *entry) const override;

    /** Splits [word] into k + 1 parts and stores these parts in wordPartBuf. */
    void storeWordPartsInBuffers(const std::string &word);

    inline static size_t getPartSize(size_t wordSize);

    /** Creates a new entry containing [wordParts] of size [partsSize].
     * They are missing [iPart] out of [0, k] parts. */
    static char *createEntry(const char *wordParts, size_t partsSize, size_t iPart);

    /** Adds [wordParts] of size [partsSize] to an existing entry pointed to by [entryPtr].
     * They are missing [iPart] out of [0, k] parts. */
    void addToEntry(char **entryPtr, const char *wordParts, size_t partsSize, size_t iPart);

    size_t calcEntryNWords(const char *entry) const;

    std::string tryMatchPart(const std::string &query, const char *entry,
                             size_t matchSize, size_t iPart) const;

    /** Sets bits for word index [iWord] and part index [iPart] in [entry].
     * iPart = 0 -> 0,0
     * iPart = 1 -> 0,1
     * iPart = 2 -> 1,0
     * iPart = 3 -> 1,1 */
    static void setPartBits(char *entry, size_t iWord, size_t iPart);
    /** Retrieves bits for word index [iWord] in [entry]. */
    static size_t retrievePartIndexFromBits(const char *entry, size_t iWord);

    /** Temporarily stores all word parts. */
    char *wordPartBuf[k + 1];
    /** Temporarily stores all word part sizes. */
    size_t wordPartSizes[k + 1];

    /** Temporarily stores remaining word parts in a contiguous fashion. */
    char *remainingWordPartsBuf = nullptr;

    /** We store 2 bits (4 positions) per word, so we can handle at most 3 errors. */
    static constexpr const size_t maxK = 3;

    SPLIT_INDEX_K_WHITEBOX
};

template<size_t k>
SplitIndexK<k>::SplitIndexK(const std::unordered_set<std::string> &wordSet,
                            hash_functions::HashFunctions::HashType hashType, float maxLoadFactor)
    :SplitIndex(wordSet)
{
    if (k < 1 or k > maxK)
    {
        throw std::invalid_argument("k must be between 1 and " + std::to_string(maxK));
    }

    const int nBucketsHint = std::max(1, static_cast<int>(nBucketsHintFactor * wordSet.size()));
    auto calcEntrySizeB = std::bind(&SplitIndexK<k>::calcEntrySizeB, this, std::placeholders::_1);

    hashMap = new hash_map::HashMapAligned(calcEntrySizeB, maxLoadFactor, nBucketsHint, hashType);

    for (size_t i = 0; i < k + 1; ++i)
    {
        wordPartBuf[i] = new char[maxWordSize];
    }

    remainingWordPartsBuf = new char[maxWordSize];
}

template<size_t k>
SplitIndexK<k>::~SplitIndexK()
{
    for (size_t i = 0; i < k + 1; ++i)
    {
        delete[] wordPartBuf[i];
    }

    delete[] remainingWordPartsBuf;
}

template<size_t k>
std::string SplitIndexK<k>::toString() const
{
    return SplitIndex::toString() + "\n(generic) k = " + std::to_string(k);
}

template<size_t k>
void SplitIndexK<k>::initEntry(const std::string &word)
{
    storeWordPartsInBuffers(word);
    
    const size_t partSize = getPartSize(word.size());
    assert(partSize >= 1 and partSize < word.size());

    for (size_t iPart = 0; iPart < k + 1; ++iPart)
    {
        assert(wordPartSizes[iPart] > 0);

        size_t start = iPart * partSize;
        size_t remainingWordPartsSize;

        if (iPart != k)
        {
            memcpy(remainingWordPartsBuf, word.c_str(), start);

            const size_t lastPartSize = word.size() - start - partSize;
            memcpy(remainingWordPartsBuf + start, word.c_str() + start + partSize, lastPartSize);

            remainingWordPartsSize = start + lastPartSize;
            assert(remainingWordPartsSize == word.size() - partSize);
        }
        else
        {
            memcpy(remainingWordPartsBuf, word.c_str(), start);
            remainingWordPartsSize = start;
        }

        char **entryPtr = hashMap->retrieve(wordPartBuf[iPart], wordPartSizes[iPart]);

        if (entryPtr == nullptr)
        {
            char *newEntry = createEntry(remainingWordPartsBuf, remainingWordPartsSize, iPart);
            hashMap->insert(wordPartBuf[iPart], wordPartSizes[iPart], newEntry);

            free(newEntry); // The entry is copied inside the map, so it can be freed here.
        }
        else
        {
            addToEntry(entryPtr, remainingWordPartsBuf, remainingWordPartsSize, iPart);
        }
    }
}

template<size_t k>
void SplitIndexK<k>::processQuery(const std::string &query, std::set<std::string> &results)
{
    assert(constructed);
    assert(query.size() > k and query.size() <= maxWordSize);

    storeWordPartsInBuffers(query);

    for (size_t iPart = 0; iPart < k + 1; ++iPart)
    {
        char **entryPtr = hashMap->retrieve(wordPartBuf[iPart], wordPartSizes[iPart]);

        if (entryPtr == nullptr)
        {
            continue;
        }

        const char *entry = *entryPtr;
        const char cMatchSize = query.size() - wordPartSizes[iPart];

        size_t iWord = 0;
        entry += sizeof(uint16_t) + *reinterpret_cast<const uint16_t *>(entry);

        while (*entry != 0)
        {
            if (*entry == cMatchSize)
            {
                if (iPart == retrievePartIndexFromBits(*entryPtr, iWord))
                {
                    const std::string result = tryMatchPart(query, entry + 1, cMatchSize, iPart);

                    if (not result.empty())
                    {
                        results.insert(move(result));
                    }
                }
            }

            iWord += 1;
            entry += 1 + *entry;
        }
    }
}

template<size_t k>
size_t SplitIndexK<k>::calcEntrySizeB(const char *entry) const
{
    const char *start = entry;
    entry += sizeof(uint16_t) + *reinterpret_cast<const uint16_t *>(entry);

    while (*entry != 0)
    {
        entry += 1 + *entry;
    }

    return entry - start + 1; // This includes the terminating 0.
}

template<size_t k>
void SplitIndexK<k>::storeWordPartsInBuffers(const std::string &word)
{
    const size_t partSize = getPartSize(word.size());
    assert(partSize >= 1 and partSize < word.size());

    for (size_t iPart = 0; iPart < k + 1; ++iPart)
    {
        const size_t start = iPart * partSize;

        if (iPart != k)
        {
            std::memcpy(wordPartBuf[iPart], word.c_str() + start, partSize);
            wordPartSizes[iPart] = partSize;
        }
        else
        {
            const size_t lastPartSize = word.size() - start;

            std::memcpy(wordPartBuf[iPart], word.c_str() + start, lastPartSize);
            wordPartSizes[iPart] = lastPartSize;
        }
    }
}

template<size_t k>
size_t SplitIndexK<k>::getPartSize(size_t wordSize)
{
    return std::floor(static_cast<double>(wordSize) / (k + 1));
}

template<size_t k>
char *SplitIndexK<k>::createEntry(const char *wordParts, size_t partsSize, size_t iPart)
{
    assert(partsSize > 0 and partsSize < maxWordSize);
    const size_t newSize = sizeof(uint16_t) + 2 + partsSize + 1; 

    char *entry = static_cast<char *>(malloc(newSize));
    assert(entry != nullptr);

    // We set the part index byte counter to 1 since there is only a single byte at the beginning.
    *reinterpret_cast<uint16_t *>(entry) = 0x1u;

    *(entry + sizeof(uint16_t)) = 0x0u; // Originally no bits are set.
    setPartBits(entry, 0, iPart);
    
    entry += sizeof(uint16_t) + 1;
    *entry = static_cast<char>(partsSize);
    
    entry += 1;
    memcpy(entry, wordParts, partsSize);
    
    entry[newSize - 1] = 0;
    return entry;
}

template<size_t k>
void SplitIndexK<k>::addToEntry(char **entryPtr, const char *wordParts, size_t partsSize, size_t iPart)
{
    assert(partsSize > 0 and partsSize < maxWordSize);
    const uint16_t nPartBytes = *reinterpret_cast<const uint16_t *>(*entryPtr);

    const size_t oldSize = calcEntrySizeB(*entryPtr);
    const size_t oldNWords = calcEntryNWords(*entryPtr);

    assert(oldNWords <= nPartBytes * 4);

    size_t newSize = oldSize + 1 + partsSize;
    bool addNewPartByte = false;

    // This means that all part bytes have been exhausted and we need to add a new one.
    if (oldNWords == nPartBytes * 4)
    {
        newSize += 1;
        addNewPartByte = true;
    }

    char *newEntry = static_cast<char *>(realloc(*entryPtr, newSize));
    assert(newEntry != nullptr);

    // TODO
}

template<size_t k>
size_t SplitIndexK<k>::calcEntryNWords(const char *entry) const
{
    size_t nWords = 0;
    entry += sizeof(uint16_t) + *reinterpret_cast<const uint16_t *>(entry);

    while (*entry != 0)
    {
        nWords += 1;
        entry += 1 + *entry;
    }

    return nWords;
}

template<size_t k>
std::string SplitIndexK<k>::tryMatchPart(const std::string &query, const char *entry,
                                         size_t matchSize, size_t iPart) const
{
    switch (iPart)
    {
        case 0:
            if (utils::Distance::isHammingAtMostK<k>(entry, query.c_str() + wordPartSizes[0], matchSize))
            {
                return std::string(query.c_str(), wordPartSizes[0]) + std::string(entry, matchSize);
            }
        case 1:
            if (k == 1)
            {

            }
            else
            {
                assert(k == 2 or k == 3);
            }

            break;
        
        case 2:
            if (k == 2)
            {

            }
            else
            {
                assert(k == 3);
            }

            break;

        case 3:
            assert(k == 3);

            if (utils::Distance::isHammingAtMostK<k>(entry, query.c_str(), matchSize))
            {
                return std::string(entry, matchSize) + std::string(query.c_str(), matchSize);
            }
        default:
            assert(false);
    }

    return "";
}

template<size_t k>
void SplitIndexK<k>::setPartBits(char *entry, size_t iWord, size_t iPart)
{
    assert(iPart >= 0 and iPart < k + 1);

    const size_t pos = iWord * 2;

    const size_t iByte = pos / 8; 
    const size_t iBit = pos % 8;

    entry += sizeof(uint16_t); // Go to the first byte.
    entry += iByte; // Go to the requested byte.

    const char mask = (iPart << iBit);
    *entry |= mask;

    assert(retrievePartIndexFromBits(entry - sizeof(uint16_t) - iByte, iWord) == iPart);
}

template<size_t k>
size_t SplitIndexK<k>::retrievePartIndexFromBits(const char *entry, size_t iWord)
{
    assert(iPart >= 0 and iPart < k + 1);
    const size_t pos = iWord * 2;

    const size_t iByte = pos / 8; 
    const size_t iBit = pos % 8;

    entry += sizeof(uint16_t); // Go to the first byte.
    entry += iByte; // Go to the requested byte.

    const size_t first = (*entry & (0x1u << iBit)) >> iBit;
    const size_t second = (*entry & (0x1u << (iBit + 1))) >> iBit;

    return first | second;
}

} // namespace split_index

#endif // SPLIT_INDEX_K_HPP
