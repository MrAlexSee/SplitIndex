#include <algorithm>
#include <boost/format.hpp>
#include <cassert>
#include <iostream>

#include "split_index_1_comp.hpp"

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

    // We sort in order to obtain the highest counts in front.
    std::partial_sort(sorter.begin(), sorter.begin() + nQgrams, sorter.end(),
        std::greater<pair<int, string>>());

    vector<string> ret;

    for (const auto &pair : sorter)
    {
        ret.push_back(pair.second);
    }

    return ret;
}

} // namespace split_index
