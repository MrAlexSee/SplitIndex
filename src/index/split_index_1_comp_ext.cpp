#include <boost/format.hpp>
#include <cstring>
#include <iostream>
#include <utility>

#include "../utils/distance.hpp"
#include "../utils/helpers.hpp"

#include "split_index_1_comp_ext.hpp"

using namespace split_index;
using namespace std;

SplitIndex1CompExt::SplitIndex1CompExt(const vector<string> &words, int minWordLength)
    :SplitIndex1Comp(words, minWordLength)
{

}

string SplitIndex1CompExt::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }
    
    string out = (boost::format("\n%1%-grams (w. length info), #q-grams = %2%")
                  % qgramSize % nQgrams).str();

    return SplitIndex1::toString() + out;
}

void SplitIndex1CompExt::initEntry(const string &word)
{
    splitWord(word);

    // Part - suffix
    char **entryPtr = map->retrieve(part1Buf, part1Size);
    size_t encSize = encode(part2Buf, part2Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntryExt(codingBuf, encSize, part2Size, true);
        map->insert(part1Buf, part1Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntryExt(entryPtr, codingBuf, encSize, part2Size, true);
    }

    // Part - prefix
    entryPtr = map->retrieve(part2Buf, part2Size);
    encSize = encode(part1Buf, part1Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntryExt(codingBuf, encSize, part1Size, false);
        map->insert(part2Buf, part2Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntryExt(entryPtr, codingBuf, encSize, part1Size, false);
    }
}

char *SplitIndex1CompExt::createEntryExt(const char *wordPart, size_t partSize,
                                         size_t decSize, bool isPartSuffix) const
{
    assert(partSize > 0 and partSize <= maxWordSize);

    size_t newSize = 4 + partSize + 1;
    char *entry = static_cast<char *>(malloc(newSize));

    // The index is a 1-based index to the first prefix part.
    if (isPartSuffix == false)
    {
        *reinterpret_cast<uint16_t *>(entry) = 0x1;
    }
    else
    {
        *reinterpret_cast<uint16_t *>(entry) = 0x0;
    }

    entry[2] = static_cast<char>(partSize);
    entry[3] = decSize;

    for (size_t i = 0; i < partSize; ++i)
    {
        entry[4 + i] = wordPart[i];
    }
    
    assert(4 + partSize == newSize - 1);
    entry[newSize - 1] = '\0';

    return entry;
}

void SplitIndex1CompExt::addToEntryExt(char **entryPtr, const char *wordPart, size_t partSize,
                                       size_t decSize, bool isPartSuffix) const
{
    assert(partSize > 0 and partSize <= maxWordSize);
    size_t oldSize = calcEntrySize(*entryPtr);
    
    size_t newSize = oldSize + partSize + 2;
    
    char *newEntry = static_cast<char *>(realloc(*entryPtr, newSize));
    assert(newEntry != nullptr);

    // The index is a 1-based index to the first prefix part.
    uint16_t *prefIndex = reinterpret_cast<uint16_t *>(newEntry);

    if (isPartSuffix and (*prefIndex) != 0)
    {
        // We determine where the prefixes start.
        char *prefStart = advanceByWords(newEntry + 2, (*prefIndex) - 1);

        // We move all prefixes to the right and thus make space for the new suffix.
        size_t offset = prefStart - newEntry;
        assert(oldSize - offset >= 0);

        moveToRight(prefStart, partSize + 2, oldSize - offset);

        // We put the new word (suffix) in the place where old prefixes used to begin.
        prefStart[0] = static_cast<char>(partSize);
        prefStart[1] = static_cast<char>(decSize);

        for (size_t i = 0; i < partSize; ++i)
        {
            prefStart[2 + i] = wordPart[i];
        }

        // We added a suffix (to the 1st part), so now the prefixes (the 2nd part) start one word further.
        *prefIndex += 1;
    }
    else
    // We append in two cases:
    // 1. a prefix (to the 2nd part of the list)
    // 2. a suffix and there are no prefixes (to the 1st part of the list)
    {
        appendToEntryExt(newEntry, oldSize, wordPart, partSize, decSize);

        // This is true only if we add a prefix for the first time (there were only suffixes before).
        if (isPartSuffix == false and *prefIndex == 0)
        {
            // Old number of words + 1, no need for "+1" since we already inserted a new word.
            *prefIndex = calcEntryNWords(newEntry);
        }
    }

    // Required in the case the memory has been moved, no need for free (realloc does that).
    *entryPtr = newEntry;
    assert(newEntry[newSize - 1] == '\0');
}

void SplitIndex1CompExt::appendToEntryExt(char *entry, size_t oldSize, const char *wordPart,
                                          size_t partSize, size_t decSize) const
{
    entry[oldSize - 1] = static_cast<char>(partSize);
    entry[oldSize] = static_cast<char>(decSize);

    for (size_t i = 0; i < partSize; ++i)
    {
        entry[oldSize + 1 + i] = wordPart[i];
    }

    entry[oldSize + partSize + 1] = '\0';
}

bool SplitIndex1CompExt::searchPartPref(const char *keyPart, size_t keySize,
                                        const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = map->retrieve(keyPart, keySize);

    if (entryPtr == nullptr)
        return false;

    const char *entry = *entryPtr;

    // We search using the query's prefix, so we try to match suffixes here.
    const uint16_t *prefIndex = reinterpret_cast<const uint16_t *>(entry);

    // Lists have sometimes only prefixes, so this check is likely to provide some speedup.
    if (*prefIndex == 1)
        return false;

    bool hasResults = false;
    char cMatchSize = static_cast<char>(matchSize);
    
    entry += 2;

    // We have to stop when we reach the prefixes.
    if (*prefIndex != 0)
    {
        const char *end = advanceByWords(entry, *prefIndex - 1);

        while (entry != end)
        {           
            assert(entry[0] != 0);

            if (entry[1] == cMatchSize)
            {
                size_t decSize = decode(entry + 2, entry[0], matchSize);

                if (decSize == matchSize and
                    utils::Distance::isHammingAtMostK<1>(codingBuf, matchPart, matchSize))
                {
                    addResult(codingBuf, matchSize, results, '0');
                    hasResults = true;
                }
            }

            entry += 2 + entry[0];
        }
    }
    // Otherwise we simply check everything.
    else
    {
        while (entry[0] != 0)
        {       
            if (entry[1] == cMatchSize)
            {
                size_t decSize = decode(entry + 2, entry[0], matchSize);

                if (decSize == matchSize and
                    utils::Distance::isHammingAtMostK<1>(codingBuf, matchPart, matchSize))
                {
                    addResult(codingBuf, matchSize, results, '0');
                    hasResults = true;
                }
            }

            entry += 2 + entry[0];
        }
    }

    return hasResults;
}

bool SplitIndex1CompExt::searchPartSuf(const char *keyPart, size_t keySize,
                                       const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = map->retrieve(keyPart, keySize);

    if (entryPtr == nullptr)
        return false;

    const char *entry = *entryPtr;

    // We search using the query's suffix, so we try to match prefixes here.
    const uint16_t *prefIndex = reinterpret_cast<const uint16_t *>(entry);

    if (*prefIndex == 0) // Only suffixes there.
        return false;

    entry = advanceByWords(entry + 2, (*prefIndex) - 1);

    bool hasResults = false;
    char cMatchSize = static_cast<char>(matchSize);

    while (entry[0] != 0)
    {
        if (entry[1] == cMatchSize)
        {
            size_t decSize = decode(entry + 2, entry[0], matchSize);

            if (decSize == matchSize and
                utils::Distance::isHammingAtMostK<1>(codingBuf, matchPart, matchSize))
            {
                addResult(codingBuf, matchSize, results, '1');
                hasResults = true;
            }
        }

        entry += 2 + entry[0];
    }

    return hasResults;
}

char *SplitIndex1CompExt::advanceByWords(char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 2 + entry[0];
    }
     
    return entry;
}

const char *SplitIndex1CompExt::advanceByWords(const char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 2 + entry[0];
    }
     
   return entry;
}

size_t SplitIndex1CompExt::calcEntrySize(const char *entry) const
{
    const char *start = entry;
    entry += 2; // Prefix index (uint16_t)

    while (entry[0] != 0)
    {
        entry += 2 + entry[0];
    }

    return entry - start + 1; // Including the terminating '\0'.
}

int SplitIndex1CompExt::calcEntryNWords(const char *entry) const
{
    int nWords = 0;
    entry += 2; // Prefix index

    while (entry[0] != 0)
    {
        nWords += 1;
        entry += 2 + entry[0];
    }

    return nWords;
}

string SplitIndex1CompExt::entryToString(const char *entry) const
{
    string out = "|";

    // Prefix index
    out += "<" + to_string(*reinterpret_cast<const uint16_t *>(entry)) + ">";
    entry += 2;

    // Words
    while (entry[0] != 0)
    {
        out += to_string(static_cast<int>(entry[0]));
        out += to_string(static_cast<int>(entry[1]));

        for (size_t i = 1; i < static_cast<size_t>(entry[0]) + 1; ++i)
        {
            out += entry[1 + i];
        }

        entry += 2 + entry[0];
    }

    return out + "|";
}
