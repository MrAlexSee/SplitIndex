#include <boost/format.hpp>
#include <cassert>
#include <iostream>

#include "split_index.hpp"
#include "../utils/helpers.hpp"

using namespace std;

SplitIndex::SplitIndex(const vector<string> &words, int minWordLength)
    :wordSet(words.begin(), words.end()),
     minWordLength(minWordLength)
{
    assert(minWordLength > 0);
    assert(wordSet.size() > 0);
}

SplitIndex::~SplitIndex()
{
    delete map;  
}

string SplitIndex::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }

    double wordsSizeKB = getWordsSizeB() / 1024.0;
    wordsSizeKB = Helpers::round2Places(wordsSizeKB);

    return (boost::format("k = 1, #words = %1%, words size = %2% KB\n%3%")
            % wordSet.size() % wordsSizeKB % map->toString()).str();
}

void SplitIndex::construct()
{
    int nWords = wordSet.size();

    for (auto it = wordSet.begin(); it != wordSet.end(); )
    {
        if (static_cast<int>(it->size()) < minWordLength)
        {
            it = wordSet.erase(it);
        }
        else
        {
            ++it;
        }
    }

    cout << boost::format("Filtered from %1% to %2% words (length >= %3%)")
            % nWords % wordSet.size() % minWordLength << endl;

    initMap();
    constructed = true;
}

long long SplitIndex::runQueries(const vector<string> &queries, string &results)
{
    assert(constructed);
    int totalQSize = 0;

    for (const string &query : queries)
    {
        totalQSize += query.size();
    }

    results.reserve(totalQSize);

    struct timespec start, end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    for (size_t iQ = 0; iQ < queries.size(); ++iQ)
    {
        processQuery(queries[iQ], results);
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    return (end.tv_sec  - start.tv_sec)  * 1000000.0 +
           (end.tv_nsec - start.tv_nsec) / 1000.0;
}

long SplitIndex::getWordsSizeB() const
{
    long total = 0;

    for (const string &word : wordSet)
    {
        total += word.size();
    }

    return total;
}

void SplitIndex::initMap()
{
    map->clear(wordSet.size() * sizeHintFactor);
    int i = 1;

    for (const string &word : wordSet)
    {
        Helpers::printProgress("Initializing the hash map", i++, wordSet.size());
        
        assert(word.size() > 0 and word.size() <= maxWordSize);
        initEntry(word);
    }
}

void SplitIndex::addResult(const char *str, size_t size,
                           string &results, char iPart)
{   
    results.append(str, size);

    results.append(1, iPart);
    results.append(1, ' ');
}

void SplitIndex::moveToRight(char *str, size_t nPlaces, size_t size)
{
    // We start from the right in order to avoid overwriting.
    for (int i = size - 1; i >= 0; --i)
    {
        str[i + nPlaces] = str[i];
    }

    assert(str[size - 1 + nPlaces] == '\0');
}
