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
    hash_functions::HashFunctions::HashType hashType)
    :SplitIndex(wordSet)
{
    auto calcEntrySizeB = std::bind(&SplitIndex1::calcEntrySizeB, this, std::placeholders::_1);
    const int nBucketsHint = wordSet.size() * nBucketsHintFactor;

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
    entry += 2; // We jump over the prefix index (uint16_t = 2 bytes).

    while (*entry != 0)
    {
        entry += 1 + *entry;
    }

    return entry - start + 1; // This includes the terminating '\0'.
}

void SplitIndex1::storePrefixSuffixInBuffers(const string &word)
{
    prefixSize = prefixSizeLUT[word.size()];
    assert(prefixSize > 0 and prefixSize <= word.size() - 1);

    suffixSize = word.size() - prefixSize;
    assert(abs(static_cast<int>(prefixSize) - static_cast<int>(suffixSize)) <= 1);

    strncpy(prefixBuf, word.c_str(), prefixSize);
    prefixBuf[prefixSize] = '\0';

    strcpy(suffixBuf, word.c_str() + prefixSize);
    assert(suffixBuf[suffixSize] == '\0');
}

char *SplitIndex1::createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const
{
    const size_t newSize = 3 + partSize + 1;
    char *entry = static_cast<char *>(malloc(newSize));

    // We set the index which points to the first prefix in the entry.
    // It is a 1-based index over the word count.
    if (isPartSuffix == false)
    {
        *reinterpret_cast<uint16_t *>(entry) = 1u;
    }
    else
    {
        *reinterpret_cast<uint16_t *>(entry) = 0u;
    }

    entry[2] = static_cast<char>(partSize);
    memcpy(entry + 3, wordPart, partSize);

    entry[newSize - 1] = '\0';
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
        // We determine where the prefixes start.
        char *prefStart = advanceInEntryByWordCount(newEntry + 2, (*prefixIndex) - 1);

        // We move all prefixes to the right and thus make space for the new suffix.
        size_t offset = prefStart - newEntry;
        assert(oldEntrySize - offset >= 0);

        memmove(prefStart + partSize + 1, prefStart, oldEntrySize - offset);

        // We put the new word (suffix) in the place where old prefixes used to begin.
        prefStart[0] = static_cast<char>(partSize);

        for (size_t i = 0; i < partSize; ++i)
        {
            prefStart[1 + i] = wordPart[i];
        }

        // We added a suffix (to the 1st part), so now the prefixes (the 2nd part) start one word further.
        *prefixIndex += 1;
    }
    else
    // We append in two cases:
    // 1. a prefix (to the 2nd part of the list)
    // 2. a suffix and there are no prefixes (to the 1st part of the list)
    {
        appendToEntry(newEntry, oldEntrySize, wordPart, partSize);

        // This is true only if we add a prefix for the first time (there were only suffixes before).
        if (isPartSuffix == false and *prefixIndex == 0)
        {
            // Old number of words + 1, no need for "+1" since we already inserted a new word.
            *prefixIndex = calcEntryNWords(newEntry);
        }
    }

    // Required in the case the memory has been moved, no need for free (realloc does that).
    *entryPtr = newEntry;
    assert(newEntry[newSize - 1] == '\0');
}

void SplitIndex1::appendToEntry(char *entry, size_t oldSize, const char *wordPart, size_t partSize) const
{
    entry[oldSize - 1] = static_cast<char>(partSize);

    for (size_t i = 0; i < partSize; ++i)
    {
        entry[oldSize + i] = wordPart[i];
    }

    entry[oldSize + partSize] = '\0';
}

char *SplitIndex1::advanceInEntryByWordCount(char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 1 + entry[0];
    }
     
    return entry;
}

const char *SplitIndex1::advanceInEntryByWordCount(const char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 1 + entry[0];
    }
     
    return entry;
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

    // This check whether the list contains only prefixes (i.e. no suffixes that we are looking for)
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

    // This check whether the list contains only suffixes (i.e. no prefixes that we are looking for)
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

int SplitIndex1::calcEntryNWords(const char *entry) const
{
    int nWords = 0;
    entry += 2; // Prefix index

    while (entry[0] != 0)
    {
        nWords += 1;
        entry += 1 + entry[0];
    }

    return nWords;
}

string SplitIndex1::entryToString(const char *entry) const
{
    string out = "|";

    // Prefix index
    out += "<" + to_string(*reinterpret_cast<const uint16_t *>(entry)) + ">";
    entry += 2;

    // Words
    while (entry[0] != 0)
    {
        out += to_string(static_cast<int>(entry[0]));

        for (size_t i = 1; i < static_cast<size_t>(entry[0]) + 1; ++i)
        {
            out += entry[i];
        }

        entry += 1 + entry[0];
    }

    return out + "|";
}

} // namespace split_index
