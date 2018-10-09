#include <boost/format.hpp>
#include <cassert>
#include <iostream>

#include "string_utils.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

string StringUtils::getElapsedInfo(long long elapsedUs, int nQueries)
{
    string countStr;

    if (nQueries < 1000000)
    {
        const double nQueriesK = static_cast<double>(nQueries) / 1000.0;
        countStr = (boost::format("%1%k") % nQueriesK).str();
    }
    else
    {
        const double nQueriesM = static_cast<double>(nQueries) / 1000000.0;
        countStr = (boost::format("%1%M") % nQueriesM).str();
    }

    const double elapsedMs = static_cast<double>(elapsedUs) / 1000.0; // milliseconds (ms)
    const double elapsedUsPerQuery = static_cast<double>(elapsedUs) / nQueries; // microseconds (us)

    return (boost::format("Elapsed = %1%ms (%2% queries), per query = %3%us")
        % elapsedMs % countStr % elapsedUsPerQuery).str();
}

void StringUtils::printProgress(const string &info, int count, int size)
{
    assert(count >= 0 and count <= size);
    const double perc = static_cast<double>(count) * 100.0 / size;

    cout << boost::format("\r%1%: %2%/%3% (%4%%%)")
        % info % count % size % round(perc) << flush;
}

void StringUtils::filterWords(vector<string> &words, int minWordLength)
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
