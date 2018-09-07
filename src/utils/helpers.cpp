#include <boost/format.hpp>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "helpers.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

string Helpers::getTimesStr(long long elapsedUs, int nQueries)
{
    string countStr;

    if (nQueries < 1000000)
    {
        double nQueriesK = static_cast<double>(nQueries) / 1000.0;
        countStr = (boost::format("%1%k") % round2Places(nQueriesK)).str();
    }
    else
    {
        double nQueriesM = static_cast<double>(nQueries) / 1000000.0;
        countStr = (boost::format("%1%M") % round2Places(nQueriesM)).str();
    }

    double elapsedMs   = static_cast<double>(elapsedUs) / 1000.0; // Milliseconds (ms)
    double elapsedUsPQ = static_cast<double>(elapsedUs) / nQueries; // Per query, microseconds (us)

    return (boost::format("Elapsed = %1%ms (%2% queries), per query = %3%us")
            % round2Places(elapsedMs) % countStr % round2Places(elapsedUsPQ)).str();
}

void Helpers::printProgress(const string &info, int count, int size)
{
    assert(count >= 0 and count <= size);

    double perc = static_cast<double>(count) * 100.0 / size;

    cout << boost::format("\r%1%: %2%/%3% (%4%%%)")
            % info % count % size % round(perc) << flush;
}

} // namespace utils

} // namespace split_index
