#include <boost/format.hpp>
#include <cassert>
#include <iostream>

#include "string_utils.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

string StringUtils::getElapsedInfo(long long elapsedUs, int nIter, int nQueries)
{
    assert(elapsedUs > 0.0 and nIter > 0 and nQueries > 0);
    string queryCountStr;

    if (nQueries < 1000000)
    {
        const double nQueriesK = static_cast<double>(nQueries) / 1000.0;
        queryCountStr = (boost::format("%1%k") % nQueriesK).str();
    }
    else
    {
        const double nQueriesM = static_cast<double>(nQueries) / 1000000.0;
        queryCountStr = (boost::format("%1%M") % nQueriesM).str();
    }

    const double elapsedPerIterUs = elapsedUs / static_cast<double>(nIter);

    const double elapsedPerIterMs = elapsedPerIterUs / 1000.0; // milliseconds (ms)
    const double elapsedPerQueryUs = elapsedPerIterUs / nQueries; // microseconds (us)

    return (boost::format("Elapsed per iter = %1%ms, per query = %2%us (#iter = %3%, #queries = %4%)")
        % elapsedPerIterMs % elapsedPerQueryUs % nIter % queryCountStr).str();
}

void StringUtils::printProgress(string info, int count, int size)
{
    assert(count >= 0 and count <= size);
    const double perc = count * 100.0 / size;

    cout << boost::format("\r%1%: %2%/%3% (%4%%%)")
        % info % count % size % round(perc) << flush;
}

void StringUtils::filterWordsByMinLength(vector<string> &words, int minWordLength)
{
    cout << boost::format("Filtering #words = %1%, min length = %2%")
        % words.size() % minWordLength << endl;

    for (auto it = words.begin(); it != words.end(); )
    {
        if (it->size() < minWordLength)
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
