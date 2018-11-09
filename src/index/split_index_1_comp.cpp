#include <algorithm>
#include <boost/format.hpp>
#include <cassert>
#include <cstring>
#include <iostream>

#include "split_index_1_comp.hpp"
#include "../utils/distance.hpp"

using namespace std;

namespace split_index
{

SplitIndex1Comp::SplitIndex1Comp(const unordered_set<string> &wordSet,
    hash_functions::HashFunctions::HashType hashType,
    float maxLoadFactor)
        :SplitIndex1(wordSet, hashType, maxLoadFactor)
{
    codingBuf = new char[maxWordSize];
}

SplitIndex1Comp::~SplitIndex1Comp()
{
    delete[] codingBuf;
}

string SplitIndex1Comp::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }

    string out = (boost::format("\nWith compression: #%1%-grams = %2%") % qgramSize % qgramToChar.size()).str();
    return SplitIndex1::toString() + out;
}

void SplitIndex1Comp::construct()
{
    fillQgramMaps();
    SplitIndex1::construct();
}

void SplitIndex1Comp::fillQgramMaps()
{
    qgramToChar.clear();
    charToQgram.clear();

    const vector<string> qgrams = calcQGramsOrderedByFrequency();
    const size_t curNQgrams = std::min(qgrams.size(), nQgrams);

    for (size_t i = 0; i < curNQgrams; ++i)
    {
        const char curIndex = firstChar + i;
        assert(curIndex < 255u);

        charToQgram[curIndex] = qgrams[i];
        qgramToChar[qgrams[i]] = curIndex;
    }

    assert(qgramToChar.size() == curNQgrams);
    assert(charToQgram.size() == curNQgrams);

    cout << boost::format("Filled maps for %1% %2%-grams") % curNQgrams % qgramSize << endl;
}

vector<string> SplitIndex1Comp::calcQGramsOrderedByFrequency() const
{
    map<string, int> counter;

    for (const string &word : wordSet)
    {
        const size_t end = word.size() - qgramSize + 1;

        for (size_t i = 0; i < end; ++i)
        {
            assert(i + qgramSize <= word.size());
            const string qgram = word.substr(i, qgramSize);

            assert(qgram.size() == qgramSize);
            auto it = counter.find(qgram);

            if (it != counter.end())
            {
                counter[qgram] += 1;
            }
            else
            {
                counter[qgram] = 1;
            }
        }
    }

    vector<pair<int, string>> sorter; // Pairs: [count, qgram].

    for (const auto &kv : counter)
    {
        sorter.emplace_back(kv.second, kv.first);
    }

    const size_t curNQgrams = std::min(sorter.size(), nQgrams);

    // We sort in order to obtain the highest counts in front.
    std::partial_sort(sorter.begin(), sorter.begin() + curNQgrams, sorter.end(),
        std::greater<pair<int, string>>());

    vector<string> ret;

    for (const auto &pair : sorter)
    {
        ret.push_back(pair.second);
    }

    return ret;
}

void SplitIndex1Comp::initEntry(const string &word)
{
    storePrefixSuffixInBuffers(word);

    // 1. We store the pair [prefix] -> [suffix].
    char **entryPtr = hashMap->retrieve(prefixBuf, prefixSize);
    const size_t encodedSuffixSize = encodeToBuf(suffixBuf, suffixSize);

    if (entryPtr == nullptr)
    {
        char *newEntry = createEntry(codingBuf, encodedSuffixSize, true);
        hashMap->insert(prefixBuf, prefixSize, newEntry);

        free(newEntry); // The entry is copied inside the map, so it can be freed here.
    }
    else
    {
        addToEntry(entryPtr, codingBuf, encodedSuffixSize, true);
    }

    // 2. We store the pair [suffix] -> [prefix].
    entryPtr = hashMap->retrieve(suffixBuf, suffixSize);
    const size_t encodedPrefixSize = encodeToBuf(prefixBuf, prefixSize);

    if (entryPtr == nullptr)
    {
        char *newEntry = createEntry(codingBuf, encodedPrefixSize, false);
        hashMap->insert(suffixBuf, suffixSize, newEntry);

        free(newEntry); // The entry is copied inside the map, so it can be freed here.
    }
    else
    {
        addToEntry(entryPtr, codingBuf, encodedPrefixSize, false);
    }
}

void SplitIndex1Comp::searchWithPrefixAsKey(set<string> &results)
{
    char **entryPtr = hashMap->retrieve(prefixBuf, prefixSize);

    if (entryPtr == nullptr)
    {
        return;
    }

    const char *entry = *entryPtr;
    // We search with the query's prefix as key, so we shall try to match suffixes.
    const uint16_t *prefixIndex = reinterpret_cast<const uint16_t *>(entry);

    // This check whether the entry contains only prefixes (i.e. no suffixes that we are looking for)
    // is likely to provide some speedup.
    if (*prefixIndex == 1)
    {
        return;
    }

    entry += 2;
    const char cSuffixSize = static_cast<char>(suffixSize);

    if (*prefixIndex != 0)
    {
        // There are some prefixes stored in this entry, so we shall advance until they are reached.
        const char *end = advanceInEntryByWordCount(entry, *prefixIndex - 1);

        while (entry != end)
        {           
            assert(*entry != 0);

            if (*entry <= cSuffixSize)
            {
                const size_t decodedSize = decodeToBuf(entry + 1, *entry, cSuffixSize);

                if (decodedSize == cSuffixSize and 
                    utils::Distance::isHammingAtMostK<1>(codingBuf, suffixBuf, suffixSize))
                {
                    const string result = string(prefixBuf, prefixSize) + string(codingBuf, decodedSize);

                    if (results.find(result) == results.end())
                    {
                        results.insert(move(result));
                    }
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
            if (*entry <= cSuffixSize)
            {
                const size_t decodedSize = decodeToBuf(entry + 1, *entry, cSuffixSize);

                if (decodedSize == cSuffixSize and
                    utils::Distance::isHammingAtMostK<1>(codingBuf, suffixBuf, suffixSize))
                {
                    const string result = string(prefixBuf, prefixSize) + string(codingBuf, decodedSize);

                    if (results.find(result) == results.end())
                    {
                        results.insert(move(result));
                    }
                }
            }

            entry += 1 + *entry;
        }
    }
}

void SplitIndex1Comp::searchWithSuffixAsKey(set<string> &results)
{
    char **entryPtr = hashMap->retrieve(suffixBuf, suffixSize);

    if (entryPtr == nullptr)
    {
        return;
    }

    const char *entry = *entryPtr;
    // We search with the query's suffix as key, so we shall try to match prefixes.
    const uint16_t *prefixIndex = reinterpret_cast<const uint16_t *>(entry);

    // This check whether the entry contains only suffixes (i.e. no prefixes that we are looking for)
    // is likely to provide some speedup.
    if (*prefixIndex == 0)
    {
        return;
    }

    entry = advanceInEntryByWordCount(entry + 2, *prefixIndex - 1);
    const char cPrefixSize = static_cast<char>(prefixSize);

    while (*entry != 0)
    {
        if (*entry <= cPrefixSize)
        {
            const size_t decodedSize = decodeToBuf(entry + 1, *entry, cPrefixSize);

            if (decodedSize == cPrefixSize and
                utils::Distance::isHammingAtMostK<1>(codingBuf, prefixBuf, prefixSize))
            {
                const string result = string(codingBuf, decodedSize) + string(suffixBuf, suffixSize);

                if (results.find(result) == results.end())
                {
                    results.insert(move(result));
                }
            }
        }

        entry += 1 + *entry;
    }
}

size_t SplitIndex1Comp::encodeToBuf(const char *word, size_t wordSize)
{
    size_t iBuf = 0;
    const size_t qgramEnd = wordSize - qgramSize + 1;

    for (size_t iW = 0; iW < wordSize; ++iW)
    {
        if (iW < qgramEnd)
        {
            auto it = qgramToChar.find(string(word + iW, qgramSize));

            if (it != qgramToChar.end())
            {
                codingBuf[iBuf++] = it->second;
                iW += qgramSize - 1;
            }
            else
            {
                codingBuf[iBuf++] = word[iW];
            }
        }
        else
        {
            codingBuf[iBuf++] = word[iW];
        }
    }

    assert(iBuf < maxWordSize);
    return iBuf;
}

size_t SplitIndex1Comp::decodeToBuf(const char *word, size_t wordSize, size_t maxDecodedWordSize)
{
    size_t iBuf = 0;

    for (size_t iW = 0; iW < wordSize; ++iW)
    {
        auto it = charToQgram.find(word[iW]);

        if (it != charToQgram.end())
        {
            const string &qgram = it->second;

            memcpy(codingBuf + iBuf, qgram.c_str(), qgram.size());
            iBuf += qgram.size();
        }
        else
        {
            codingBuf[iBuf++] = word[iW];
        }

        if (iBuf > maxDecodedWordSize)
        {
            return 0;
        }
    }

    return iBuf;
}

} // namespace split_index
