#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>

#include "split_index_1.hpp"
#include "../hash_map/hash_map_aligned.hpp"
#include "../utils/distance.hpp"

using namespace std;

namespace split_index
{

SplitIndex1::SplitIndex1(const unordered_set<string> &wordSet,
    hash_functions::HashFunctions::HashType hashType,
    float maxLoadFactor)
    :SplitIndex(wordSet)
{
    auto calcEntrySizeB = std::bind(&SplitIndex1::calcEntrySizeB, this, std::placeholders::_1);
    const int nBucketsHint = std::max(1, static_cast<int>(nBucketsHintFactor * wordSet.size()));

    hashMap = new hash_map::HashMapAligned(calcEntrySizeB, maxLoadFactor, nBucketsHint, hashType);

    prefixBuf = new char[maxWordSize + 1];
    suffixBuf = new char[maxWordSize + 1];

    prefixSizeLUT = new size_t[maxWordSize + 1];
}

SplitIndex1::~SplitIndex1()
{
    delete[] prefixBuf;
    delete[] suffixBuf;

    delete[] prefixSizeLUT;
}

string SplitIndex1::toString() const
{
    string ret = "k = 1\n";
    return ret + SplitIndex::toString();
}

void SplitIndex1::construct()
{
    for (size_t i = 0; i <= maxWordSize; ++i)
    {
        prefixSizeLUT[i] = i / 2;
    }

    SplitIndex::construct();
}

void SplitIndex1::initEntry(const string &word)
{
    storePrefixSuffixInBuffers(word);

    // 1. We store the pair [prefix] -> [suffix].
    char **entryPtr = hashMap->retrieve(prefixBuf, prefixSize);

    if (entryPtr == nullptr)
    {
        char *newEntry = createEntry(suffixBuf, suffixSize, true);
        hashMap->insert(prefixBuf, prefixSize, newEntry);

        // The entry is copied inside the map, so it can be freed here.
        free(newEntry);
    }
    else
    {
        // cout << "add prefix" << endl;
        addToEntry(entryPtr, suffixBuf, suffixSize, true);
    }

    // 2. We store the pair [suffix] -> [prefix].
    entryPtr = hashMap->retrieve(suffixBuf, suffixSize);

    if (entryPtr == nullptr)
    {
        char *newEntry = createEntry(prefixBuf, prefixSize, false);
        hashMap->insert(suffixBuf, suffixSize, newEntry);

        // The entry is copied inside the map, so it can be freed here.
        free(newEntry);
    }
    else
    {
        // cout << "add suffix" << endl;
        addToEntry(entryPtr, prefixBuf, prefixSize, false);
    }
}

int SplitIndex1::processQuery(const string &query, string &results)
{
    assert(constructed);
    assert(query.size() > 0 and query.size() <= maxWordSize);

    storePrefixSuffixInBuffers(query);

    int nMatches = 0;

    nMatches += searchWithPrefixAsKey(results);
    nMatches += searchWithSuffixAsKey(results);

    // If some matches occurred, the results string shall also contain a query.
    if (nMatches > 0)
    {
        results.append(query);
        results.append(1, '\n');
    }

    return nMatches;
}

size_t SplitIndex1::calcEntrySizeB(const char *entry) const
{
    const char *start = entry;
    entry += 2; // We jump over the prefix index (uint16_t == 2 bytes).

    while (*entry != 0)
    {
        entry += 1 + *entry;
    }

    return entry - start + 1; // This includes the terminating 0.
}


int SplitIndex1::calcEntryNWords(const char *entry) const
{
    int nWords = 0;
    entry += 2; // We jump over the prefix index (uint16_t == 2 bytes).

    while (*entry != 0)
    {
        nWords += 1;
        entry += 1 + *entry;
    }

    return nWords;
}

void SplitIndex1::storePrefixSuffixInBuffers(const string &word)
{
    prefixSize = prefixSizeLUT[word.size()];
    assert(prefixSize > 0 and prefixSize <= word.size() - 1);

    suffixSize = word.size() - prefixSize;
    assert(suffixSize > 0 and suffixSize <= word.size() - 1);

    assert(abs(static_cast<int>(prefixSize) - static_cast<int>(suffixSize)) <= 1);

    strncpy(prefixBuf, word.c_str(), prefixSize);
    prefixBuf[prefixSize] = 0;

    strcpy(suffixBuf, word.c_str() + prefixSize);
    assert(suffixBuf[suffixSize] == 0);
}

char *SplitIndex1::createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const
{
    const size_t newSize = 3 + partSize + 1;
    char *entry = static_cast<char *>(malloc(newSize));

    // We set the index which points to the first prefix in the entry.
    // It is a 1-based index over the word count.
    if (not isPartSuffix)
    {
        *reinterpret_cast<uint16_t *>(entry) = 1u;
    }
    else
    {
        *reinterpret_cast<uint16_t *>(entry) = 0u;
    }

    entry[2] = static_cast<char>(partSize);
    memcpy(entry + 3, wordPart, partSize);

    entry[newSize - 1] = 0;
    return entry;
}

void SplitIndex1::addToEntry(char **entryPtr,
    const char *wordPart, size_t partSize,
    bool isPartSuffix) const
{
    assert(partSize > 0 and partSize <= maxWordSize);
    
    const size_t oldEntrySize = calcEntrySizeB(*entryPtr);
    const size_t newEntrySize = oldEntrySize + partSize + 1;
    
    char *newEntry = static_cast<char *>(realloc(*entryPtr, newEntrySize));

    // We act depending on the value of the prefix index.
    // It is a 1-based index over the word count.
    uint16_t *prefixIndex = reinterpret_cast<uint16_t *>(newEntry);

    if (isPartSuffix and (*prefixIndex) != 0)
    {
        // We want to insert a suffix and there exist some prefixes.
        // Hence, we need to move the prefixes to the right and insert a suffix somewhere in the middle.
        char *prefixesStart = advanceInEntryByWordCount(newEntry + 2, (*prefixIndex) - 1);

        const size_t prefixesListSize = oldEntrySize - (prefixesStart - newEntry);
        assert(newEntrySize == prefixesStart - newEntry + partSize + 1 + prefixesListSize);

        memmove(prefixesStart + partSize + 1, prefixesStart, prefixesListSize);

        // After moving, we can insert the suffix where old prefixes used to begin.
        *prefixesStart = static_cast<char>(partSize);
        memcpy(prefixesStart + 1, wordPart, partSize);

        // We have added a suffix (1st part of the entry), so now the prefixes (2nd part of the entry) start one word further.
        *prefixIndex += 1;
    }
    else
    {
        // Otherwise we append the word part to the entry. This is one of the two following cases:
        // a) we are adding a prefix (i.e. adding to the 2nd part of the entry),
        // b) we are adding a suffix and there are no prefixes (i.e. we are adding to the 1st, 
        // that is the only part of the entry).
        appendToEntry(newEntry, oldEntrySize, wordPart, partSize);

        // This is true only if we add a prefix for the first time (when there were only suffixes before).
        if (not isPartSuffix and *prefixIndex == 0)
        {
            // The new index is the old number of words + 1, no need for "+1" here since we have already inserted a new word.
            *prefixIndex = calcEntryNWords(newEntry);
        }
    }

    // This is required in the case the memory has been moved by realloc.
    *entryPtr = newEntry;
    assert(newEntry[newEntrySize - 1] == 0);
}

void SplitIndex1::appendToEntry(char *entry, size_t oldEntrySize,
    const char *wordPart, size_t partSize) const
{
    entry[oldEntrySize - 1] = static_cast<char>(partSize);

    memcpy(entry + oldEntrySize, wordPart, partSize);
    entry[oldEntrySize + partSize] = 0;
}

int SplitIndex1::searchWithPrefixAsKey(string &results)
{
    char **entryPtr = hashMap->retrieve(prefixBuf, prefixSize);

    if (entryPtr == nullptr)
    {
        return 0;
    }

    const char *entry = *entryPtr;
    // We search with the query's prefix as key, so we shall try to match suffixes.
    const uint16_t *prefIndex = reinterpret_cast<const uint16_t *>(entry);

    // This check whether the entry contains only prefixes (i.e. no suffixes that we are looking for)
    // is likely to provide some speedup.
    if (*prefIndex == 1)
    {
        return 0;
    }

    const char cSuffixSize = static_cast<char>(suffixSize);
    int nMatches = 0;
    
    entry += 2;

    if (*prefIndex != 0)
    {
        // There are some prefixes stored in this entry, so we shall advance until they are reached.
        const char *end = advanceInEntryByWordCount(entry, *prefIndex - 1);

        while (entry != end)
        {           
            assert(*entry != 0);

            if (*entry == cSuffixSize)
            {
                if (utils::Distance::isHammingAtMostK<1>(entry + 1, suffixBuf, suffixSize))
                {
                    addPartialResult(entry + 1, suffixSize, results, 0);
                    nMatches += 1;
                }
            }

            entry += 1 + *entry;
        }
    }
    else
    {
        // There are only suffixes stored in this entry, so we check everything.
        while (*entry != 0)
        {       
            if (*entry == cSuffixSize)
            {
                if (utils::Distance::isHammingAtMostK<1>(entry + 1, suffixBuf, suffixSize))
                {
                    addPartialResult(entry + 1, suffixSize, results, 0);
                    nMatches += 1;
                }
            }

            entry += 1 + entry[0];
        }
    }

    return nMatches;
}

int SplitIndex1::searchWithSuffixAsKey(string &results)
{
    char **entryPtr = hashMap->retrieve(suffixBuf, suffixSize);

    if (entryPtr == nullptr)
    {
        return 0;
    }

    const char *entry = *entryPtr;

    // We search with the query's suffix as key, so we shall try to match prefixes.
    const uint16_t *prefIndex = reinterpret_cast<const uint16_t *>(entry);

    // This check whether the entry contains only suffixes (i.e. no prefixes that we are looking for)
    // is likely to provide some speedup.
    if (*prefIndex == 0)
    {
        return 0;
    }

    entry = advanceInEntryByWordCount(entry + 2, *prefIndex - 1);

    const char cPrefixSize = static_cast<char>(prefixSize);
    int nMatches = 0;

    while (*entry != 0)
    {
        if (*entry == cPrefixSize)
        {
            if (utils::Distance::isHammingAtMostK<1>(entry + 1, prefixBuf, prefixSize))
            {
                addPartialResult(entry + 1, prefixSize, results, 1);
                nMatches += 1;
            }
        }

        entry += 1 + *entry;
    }

    return nMatches;
}

char *SplitIndex1::advanceInEntryByWordCount(char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(*entry != 0);
        entry += 1 + *entry;
    }
     
    return entry;
}

const char *SplitIndex1::advanceInEntryByWordCount(const char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(*entry != 0);
        entry += 1 + *entry;
    }
     
    return entry;
}

} // namespace split_index
