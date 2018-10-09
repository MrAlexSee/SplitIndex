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

    part1Buf = new char[maxWordSize];
    part2Buf = new char[maxWordSize];

    partSizeLUT = new size_t[maxWordSize + 1];
}

SplitIndex1::~SplitIndex1()
{
    delete[] part1Buf;
    delete[] part2Buf;

    delete[] partSizeLUT;
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
        partSizeLUT[i] = i / 2;
    }

    SplitIndex::construct();
}

void SplitIndex1::initEntry(const string &word)
{
    splitWord(word);

    // Part - suffix
    char **entryPtr = hashMap->retrieve(part1Buf, part1Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntry(part2Buf, part2Size, true);
        hashMap->insert(part1Buf, part1Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntry(entryPtr, part2Buf, part2Size, true);
    }

    // Part - prefix
    entryPtr = hashMap->retrieve(part2Buf, part2Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntry(part1Buf, part1Size, false);
        hashMap->insert(part2Buf, part2Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntry(entryPtr, part1Buf, part1Size, false);
    }
}

void SplitIndex1::splitWord(const string &word)
{
    size_t partSize = getPartSize(word.size());
    assert(partSize > 0 and partSize <= word.size() - 1);

    part1Size = partSize;
    part2Size = word.size() - partSize;

    strncpy(part1Buf, word.c_str(), partSize);
    part1Buf[partSize] = '\0';

    strcpy(part2Buf, word.c_str() + partSize);

    assert(part2Buf[part2Size] == '\0');
    assert(abs(static_cast<int>(part1Size) - static_cast<int>(part2Size)) <= 1);
}

char *SplitIndex1::createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const
{
    size_t newSize = 3 + partSize + 1;
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

    for (size_t i = 0; i < partSize; ++i)
    {
        entry[3 + i] = wordPart[i];
    }
    
    assert(3 + partSize == newSize - 1);
    entry[newSize - 1] = '\0';

    return entry;
}

void SplitIndex1::addToEntry(char **entryPtr, const char *wordPart, size_t partSize, bool isPartSuffix) const
{
    assert(partSize > 0 and partSize <= maxWordSize);
    size_t oldSize = calcEntrySizeB(*entryPtr);
    
    size_t newSize = oldSize + partSize + 1;
    
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

        //memmove(dest, partSize + 1, oldSize - offset); // TODO??
        //moveToRight(prefStart, partSize + 1, oldSize - offset);

        // We put the new word (suffix) in the place where old prefixes used to begin.
        prefStart[0] = static_cast<char>(partSize);

        for (size_t i = 0; i < partSize; ++i)
        {
            prefStart[1 + i] = wordPart[i];
        }

        // We added a suffix (to the 1st part), so now the prefixes (the 2nd part) start one word further.
        *prefIndex += 1;
    }
    else
    // We append in two cases:
    // 1. a prefix (to the 2nd part of the list)
    // 2. a suffix and there are no prefixes (to the 1st part of the list)
    {
        appendToEntry(newEntry, oldSize, wordPart, partSize);

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

void SplitIndex1::appendToEntry(char *entry, size_t oldSize, const char *wordPart, size_t partSize) const
{
    entry[oldSize - 1] = static_cast<char>(partSize);

    for (size_t i = 0; i < partSize; ++i)
    {
        entry[oldSize + i] = wordPart[i];
    }

    entry[oldSize + partSize] = '\0';
}

char *SplitIndex1::advanceByWords(char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 1 + entry[0];
    }
     
    return entry;
}

const char *SplitIndex1::advanceByWords(const char *entry, uint16_t nWords) const
{
    for (uint16_t i = 0; i < nWords; ++i)
    {
        assert(entry[0] != 0);
        entry += 1 + entry[0];
    }
     
    return entry;
}

int SplitIndex1::processQuery(const string &query, string &results)
{
    assert(constructed);
    assert(query.size() >= 1 and query.size() <= maxWordSize);

    splitWord(query);

    bool hasResults = false;

    if (searchPartPref(part1Buf, part1Size, part2Buf, part2Size, results))
    {
        hasResults = true;
    }

    if (searchPartSuf(part2Buf, part2Size, part1Buf, part1Size, results))
    {
        hasResults = true;
    }

    if (hasResults)
    {
        results.append(query);
        results.append(1, '\n');
    }

    return 0; // TODO
}

bool SplitIndex1::searchPartPref(const char *keyPart, size_t keySize,
                                 const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = hashMap->retrieve(keyPart, keySize);

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

    // We have to stop when we reach the "missing" prefixes.
    if (*prefIndex != 0)
    {
        const char *end = advanceByWords(entry, *prefIndex - 1);

        while (entry != end)
        {           
            assert(entry[0] != 0);

            if (entry[0] == cMatchSize)
            {
                if (utils::Distance::isHammingAtMostK<1>(entry + 1, matchPart, matchSize))
                {
                    addResult(entry + 1, matchSize, results, '0');
                    hasResults = true;
                }
            }

            entry += 1 + entry[0];
        }
    }
    // Otherwise we simply check everything.
    else
    {
        while (entry[0] != 0)
        {       
            if (entry[0] == cMatchSize)
            {
                if (utils::Distance::isHammingAtMostK<1>(entry + 1, matchPart, matchSize))
                {
                    addResult(entry + 1, matchSize, results, '0');
                    hasResults = true;
                }
            }

            entry += 1 + entry[0];
        }
    }

    return hasResults;
}

bool SplitIndex1::searchPartSuf(const char *keyPart, size_t keySize,
                                const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = hashMap->retrieve(keyPart, keySize);

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
        if (entry[0] == cMatchSize)
        {
            if (utils::Distance::isHammingAtMostK<1>(entry + 1, matchPart, matchSize))
            {
                addResult(entry + 1, matchSize, results, '1');
                hasResults = true;
            }
        }

        entry += 1 + entry[0];
    }

    return hasResults;
}

size_t SplitIndex1::calcEntrySizeB(const char *entry) const
{
    const char *start = entry;
    entry += 2; // Prefix index (uint16_t)

    while (entry[0] != 0)
    {
        entry += 1 + entry[0];
    }

    return entry - start + 1; // Including the terminating '\0'.
}

size_t SplitIndex1::getPartSize(size_t wordSize) const
{
    assert(wordSize <= maxWordSize and partSizeLUT != nullptr);
    return partSizeLUT[wordSize];
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
