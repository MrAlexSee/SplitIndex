#include <boost/format.hpp>
#include <cmath>
#include <cstring>
#include <iostream>

#include "../utils/distance.hpp"
#include "../utils/helpers.hpp"

#include "split_index_exact.hpp"

using namespace split_index;
using namespace std;

SplitIndexExact::SplitIndexExact(const vector<string> &words, int minWordLength)
    :SplitIndex1(words, minWordLength)
{

}

string SplitIndexExact::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }

    double wordsSizeKB = getWordsSizeB() / 1024.0;
    wordsSizeKB = utils::Helpers::round2Places(wordsSizeKB);

    return (boost::format("Exact (k = 0), #words = %1%, words size = %2%kB\n%3%")
            % wordSet.size() % wordsSizeKB % map->toString()).str();
}

bool SplitIndexExact::searchPartPref(const char *keyPart, size_t keySize,
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

            if (entry[0] == cMatchSize)
            {
                if (utils::Distance::isExactMatch(entry + 1, matchPart, matchSize))
                {
                    addResult(entry + 1, matchSize, results, false);
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
                if (utils::Distance::isExactMatch(entry + 1, matchPart, matchSize))
                {
                    addResult(entry + 1, matchSize, results, false);
                    hasResults = true;
                }
            }

            entry += 1 + entry[0];
        }
    }

    return hasResults;
}

bool SplitIndexExact::searchPartSuf(const char *keyPart, size_t keySize,
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
        if (entry[0] == cMatchSize)
        {
            if (utils::Distance::isExactMatch(entry + 1, matchPart, matchSize))
            {
                addResult(entry + 1, matchSize, results, true);
                hasResults = true;
            }
        }

        entry += 1 + entry[0];
    }

    return hasResults;
}
