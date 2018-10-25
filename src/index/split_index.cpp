#include <boost/format.hpp>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "split_index.hpp"
#include "../utils/string_utils.hpp"

using namespace std;

namespace split_index
{

SplitIndex::SplitIndex(const unordered_set<string> &wordSetArg)
    :wordSet(wordSetArg)
{
    if (wordSet.empty())
    {
        throw runtime_error("word set cannot be empty");
    }
}

SplitIndex::~SplitIndex()
{
    delete hashMap;
}

void SplitIndex::construct()
{
    const int nBucketsHint = std::max(1, static_cast<int>(nBucketsHintFactor * wordSet.size()));
    hashMap->clear(nBucketsHint);

    cout << "Set a hash map with hint #buckets = " << nBucketsHint << endl << endl;
    int i = 1;

    for (const string &word : wordSet)
    {
        utils::StringUtils::printProgress(string("Constructing the hash map"), i++, wordSet.size());

        assert(word.size() > 0 and word.size() <= maxWordSize);
        initEntry(word);
    }

    constructed = true;
}

string SplitIndex::toString() const
{
    if (not constructed)
    {
        throw runtime_error("index not constructed");
    }

    const float wordsSizeKB = calcWordsSizeB() / 1024.0;
    string ret = (boost::format("#words = %1%, words size = %2% KB")
        % wordSet.size() % wordsSizeKB).str();

    ret += "\n" + hashMap->toString();
    return ret;
}

set<string> SplitIndex::search(const vector<string> &queries, int nIter)
{
    assert(constructed);
    set<string> ret;

    clock_t start = std::clock();

    for (int i = 0; i < nIter; ++i)
    {
        for (const string &query : queries)
        {
            processQuery(query, ret);
        }
    }

    clock_t end = std::clock();

    const float elapsedS = (end - start) / static_cast<float>(CLOCKS_PER_SEC);
    elapsedUs = elapsedS * 1000000.0f;

    return ret;
}

set<string> SplitIndex::searchAndDumpMatchCounts(const vector<string> &queries)
{
    assert(constructed);
    set<string> ret;

    for (const string &query : queries)
    {
        set<string> curResults;
        processQuery(query, curResults);

        cout << query << " -> " << curResults.size() << endl;
        ret.insert(curResults.begin(), curResults.end());
    }

    return ret;
}

long SplitIndex::calcWordsSizeB() const
{
    long total = 0;

    for (const string &word : wordSet)
    {
        total += word.size();
    }

    return total;
}

} // namespace split_index
