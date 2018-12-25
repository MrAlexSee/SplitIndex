#include <boost/format.hpp>
#include <cassert>
#include <iostream>

#include "string_utils.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

string StringUtils::getElapsedInfo(float elapsedUs, int nIter, int nQueries)
{
    assert(elapsedUs > 0.0 and nIter > 0 and nQueries > 0);
    string queryCountStr;

    if (nQueries < 1000000)
    {
        const float nQueriesK = nQueries / 1000.0f;
        queryCountStr = (boost::format("%1%k") % nQueriesK).str();
    }
    else
    {
        const float nQueriesM = nQueries / 1000000.0f;
        queryCountStr = (boost::format("%1%M") % nQueriesM).str();
    }

    const float elapsedPerIterUs = elapsedUs / nIter;

    const float elapsedPerIterMs = elapsedPerIterUs / 1000.0; // milliseconds (ms)
    const float elapsedPerQueryUs = elapsedPerIterUs / nQueries; // microseconds (us)

    return (boost::format("Elapsed per iter = %1%ms, per query = %2%us (#iter = %3%, #queries = %4%)")
        % elapsedPerIterMs % elapsedPerQueryUs % nIter % queryCountStr).str();
}

void StringUtils::printProgress(string info, int count, int size)
{
    assert(count >= 0 and count <= size);
    const float perc = count * 100.0 / size;

    cout << boost::format("\r%1%: %2%/%3% (%4%%%)")
        % info % count % size % round(perc) << flush;
}

void StringUtils::filterWordsByMinLength(vector<string> &words, int minWordLength)
{
    cout << boost::format("Filtering #words = %1%, min length = %2%")
        % words.size() % minWordLength << endl;

    for (auto it = words.begin(); it != words.end(); )
    {
        if (it->size() < static_cast<size_t>(minWordLength))
        {
            it = words.erase(it);
        }
        else
        {
            ++it;
        }
    }

    cout << "Filtered to #words = " << words.size() << endl;
}

} // namespace utils

} // namespace split_index
