#include <boost/format.hpp>
#include <cstring>
#include <iostream>
#include <utility>

#include "../utils/dist.hpp"
#include "../utils/helpers.hpp"

#include "split_index_1_comp.hpp"

using namespace std;

SplitIndex1Comp::SplitIndex1Comp(const vector<string> &words, int minWordLength)
    :SplitIndex1(words, minWordLength)
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
    
    string out = (boost::format("\n%1%-grams, #q-grams = %2%")
                  % qgramSize % nQgrams).str();

    return SplitIndex1::toString() + out;
}

void SplitIndex1Comp::construct()
{
    constructQgramLists();
    SplitIndex1::construct();
}

void SplitIndex1Comp::constructQgramLists()
{
    qgramToChar.clear();
    charToQgram.clear();

    constructQGrams(nQgrams, qgramSize, firstChar);

    assert(qgramToChar.size() == nQgrams);
    assert(charToQgram.size() == nQgrams);
}

void SplitIndex1Comp::constructQGrams(int nQgrams, int qgramSize, char firstChar)
{
    auto order = constructQGramOrder(qgramSize);

    for (int i = 0; i < order.size() and i < nQgrams; ++i)
    {
        assert(firstChar + i < 255);

        qgramToChar[order[i]] = firstChar + i;
        charToQgram[firstChar + i] = order[i];
    }

    if (order.size() < nQgrams)
    {
        cout << boost::format("\nFewer q-grams than requested: %1% < %2%")
                % order.size() % nQgrams << endl;
    }

    cout << boost::format("Constructed lists for %1% %2%-grams")
            % nQgrams % qgramSize << endl;
}

vector<string> SplitIndex1Comp::constructQGramOrder(int qgramSize)
{
    unordered_map<string, int> counter;

    for (const string &word : wordSet)
    {
        for (int i = 0; i < static_cast<int>(word.size()) - qgramSize + 1; ++i)
        {
            assert(i + qgramSize <= static_cast<int>(word.size()));
            string qgram = word.substr(i, qgramSize);

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

    vector<pair<int, string>> sorter; // Count, qgram

    for (const auto &kv : counter)
    {
        sorter.push_back(make_pair(kv.second, kv.first));
    }

    // We want the highest count in front, hence we do a reverse search.
    std::sort(sorter.begin(), sorter.end(), std::greater<pair<int, string>>());
    assert(sorter[0].first >= sorter[1].first);

    vector<string> order;

    for (const auto &tup : sorter)
    {
        order.push_back(tup.second);
    }

    cout << boost::format("Constructed order for %1% %2%-grams")
            % order.size() % qgramSize << endl;
    return order;
}

void SplitIndex1Comp::initEntry(const string &word)
{
    splitWord(word);

    // Part - suffix
    char **entryPtr = map->get(part1Buf, part1Size);
    size_t encSize = encode(part2Buf, part2Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntry(codingBuf, encSize, true);
        map->insert(part1Buf, part1Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntry(entryPtr, codingBuf, encSize, true);
    }

    // Part - prefix
    entryPtr = map->get(part2Buf, part2Size);
    encSize = encode(part1Buf, part1Size);

    if (entryPtr == nullptr)
    {
        char *entry = createEntry(codingBuf, encSize, false);
        map->insert(part2Buf, part2Size, entry);

        free(entry); // Copied inside the map.
    }
    else
    {
        addToEntry(entryPtr, codingBuf, encSize, false);
    }
}

bool SplitIndex1Comp::searchPartPref(const char *keyPart, size_t keySize,
                                     const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = map->get(keyPart, keySize);

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

            if (entry[0] <= cMatchSize)
            {
                size_t decSize = decode(entry + 1, entry[0], matchSize);

                if (decSize == matchSize and
                    Dist::isHammingK<1>(codingBuf, matchPart, matchSize))
                {
                    addResult(codingBuf, matchSize, results, '0');
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
            if (entry[0] <= cMatchSize)
            {
                size_t decSize = decode(entry + 1, entry[0], matchSize);

                if (decSize == matchSize and
                    Dist::isHammingK<1>(codingBuf, matchPart, matchSize))
                {
                    addResult(codingBuf, matchSize, results, '0');
                    hasResults = true;
                }
            }

            entry += 1 + entry[0];
        }
    }

    return hasResults;
}

bool SplitIndex1Comp::searchPartSuf(const char *keyPart, size_t keySize,
                                    const char *matchPart, size_t matchSize, string &results)
{
    char **entryPtr = map->get(keyPart, keySize);

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
        if (entry[0] <= cMatchSize)
        {
            size_t decSize = decode(entry + 1, entry[0], matchSize);

            if (decSize == matchSize and
                Dist::isHammingK<1>(codingBuf, matchPart, matchSize))
            {
                addResult(codingBuf, matchSize, results, '1');
                hasResults = true;
            }
        }

        entry += 1 + entry[0];
    }

    return hasResults;
}

size_t SplitIndex1Comp::encode(const char *word, size_t wordSize)
{
    return encodeToBuf(word, wordSize, this->codingBuf, this->qgramSize);
}

size_t SplitIndex1Comp::decode(const char *word, size_t wordSize, size_t maxSize)
{
    return decodeToBuf(word, wordSize, this->codingBuf, maxSize);
}

size_t SplitIndex1Comp::encodeToBuf(const char *word, size_t wordSize, char *buf, int qgramSize)
{
    size_t iBuf = 0;

    for (int iW = 0; iW < wordSize; ++iW)
    {
        if (iW < static_cast<int>(wordSize) - qgramSize + 1)
        {
            assert(iW + qgramSize <= static_cast<int>(wordSize));
            auto it = qgramToChar.find(string(word + iW, qgramSize));

            if (it != qgramToChar.end())
            {
                buf[iBuf++] = it->second;
                iW += qgramSize - 1;
            }
            else
            {
                buf[iBuf++] = word[iW];
            }
        }
        else
        {
            buf[iBuf++] = word[iW];
        }
    }

    assert(iBuf < maxWordSize);
    return iBuf;
}

size_t SplitIndex1Comp::decodeToBuf(const char *word, size_t wordSize, char *buf, size_t maxSize)
{
    size_t iBuf = 0;

    for (size_t iW = 0; iW < wordSize; ++iW)
    {
        auto it = charToQgram.find(word[iW]);

        if (it != charToQgram.end())
        {
            strncpy(buf + iBuf, it->second.c_str(), it->second.size());
            iBuf += it->second.size();
        }
        else
        {
            buf[iBuf++] = word[iW];
        }

        if (iBuf > maxSize)
        {
            return 0;
        }
    }

    return iBuf;
}
