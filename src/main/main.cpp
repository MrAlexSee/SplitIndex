#include <boost/format.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../index/split_index.hpp"
#include "../index/split_index_exact.hpp"
#include "../index/split_index_1.hpp"
#include "../index/split_index_1_comp.hpp"
#include "../index/split_index_1_comp_double.hpp"
#include "../index/split_index_1_comp_ext.hpp"
#include "../index/split_index_1_comp_opt.hpp"
#include "../index/split_index_k.hpp"

#include "../utils/file_io.hpp"
#include "../utils/helpers.hpp"
#include "../utils/query_utils.hpp"
#include "params.hpp"

using namespace split_index;
using namespace std;

namespace
{
    Params params;
}

string doSearch(const vector<string> &words,
                const vector<string> &queries, int nIterations);
SplitIndex *initIndex(const vector<string> &words);

void checkAllQgrams(SplitIndex *index);

int main(int argc, char **argv)
{
    if (argc != 2 and argc != 3)
    {
        cerr << "Usage: ./main [dict] [opt: queries]" << endl;
        return 1;
    }

    try
    {
        vector<string> inputLines = utils::FileIO::readFile(argv[1]);
        cout << boost::format("Read %1% lines")
                % inputLines.size() << endl;

        vector<string> queries;

        if (argc != 3)
        {
            // No file with queries -> generate the queries.
            queries = utils::QueryUtils::genQueries(inputLines, params.queryAlphabet, params.minWordLength,
                                             params.nQueries, params.maxNErrors);        
            cout << boost::format("Generated %1% queries")
                    % params.nQueries << endl;
        }
        else
        {
            // Otherwise we read the queries from file.
            queries = utils::FileIO::readFile(argv[2]);
            cout << boost::format("Read %1% queries from file: %2%")
                    % queries.size() % argv[2] << endl;
        }

        string results = doSearch(inputLines, queries, params.nIterations);

        if (results != "")
        {
            utils::FileIO::writeFile(results, params.outFilePath);
        }
    }
    catch (std::exception &e)
    {
        cerr << "Aborting: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

string doSearch(const vector<string> &words,
                const vector<string> &queries, int nIterations)
{
    assert(nIterations > 0);
    long long elapsedUs = 0;

    SplitIndex *index = initIndex(words);

#if CHECK_ALL_QGRAMS
    checkAllQgrams(index);
    return "";
#endif

    index->construct();
    cout << endl << "Constructed the index: " << endl
         << index->toString() << endl << endl;

    string results;

    for (int i = 0; i < nIterations; ++i)
    {
        cout << boost::format("\rRunning queries, iter: %1%/%2%")
                % (i + 1) % nIterations << flush;

        elapsedUs += index->runQueries(queries, results);

        if (i != nIterations - 1)
        {
            // We only need the results from the last call, but we save them in every call
            // in order to have fair time measurements.
            results.clear();
        }
    }

    elapsedUs /= nIterations;
    cout << endl << utils::Helpers::getTimesStr(elapsedUs, queries.size()) << endl;

    results = index->prettyResults(results);

    delete index;
    return results;
}

SplitIndex *initIndex(const vector<string> &words)
{
    SplitIndex *index;

#if INDEX_TYPE == 0
    index = new SplitIndex1(words, params.minWordLength);
#elif INDEX_TYPE == 1
    index = new SplitIndex1Comp(words, params.minWordLength);
#elif INDEX_TYPE == 2
    index = new SplitIndex1CompExt(words, params.minWordLength);
#elif INDEX_TYPE == 3
    index = new SplitIndex1CompDouble(words, params.minWordLength);
#elif INDEX_TYPE == 4
    index = new SplitIndex1CompOpt(words, params.minWordLength);
#elif INDEX_TYPE == 5
    index = new SplitIndexK<1>(words, params.minWordLength);
#elif INDEX_TYPE == 6
    index = new SplitIndexK<2>(words, params.minWordLength);
#elif INDEX_TYPE == 7
    index = new SplitIndexK<3>(words, params.minWordLength);
#elif INDEX_TYPE == 8
    index = new SplitIndexExact(words, params.minWordLength);
#else
    #error Bad INDEX_TYPE
#endif

    return index;
}

void checkAllQgrams(SplitIndex *index)
{
    assert(index != nullptr and INDEX_TYPE == 4);
    
    SplitIndex1CompOpt *indexOpt = dynamic_cast<SplitIndex1CompOpt *>(index);
    
    if (indexOpt == nullptr)
    {
        throw runtime_error("Bad index type");
    }

    indexOpt->construct(); // Required to initially set the words set.

    // Make sure this matches maxQgramSize.
    vector<int> curCounts;

    for (int i = 0; i <= 100; i += 2)
    {
        curCounts.clear();
        curCounts.push_back(0); // 2-grams
        curCounts.push_back(100 - i);       // 3-grams
        curCounts.push_back(i);       // 4-grams

        cout << endl << "Cur counts = (2-, 3-, 4-grams)" << utils::Helpers::vecToStr(curCounts) << endl;

        indexOpt->setQGramCounts(curCounts);
        indexOpt->construct();

        double sizeKB = indexOpt->getTotalSizeB() / 1024.0;

        // This is sent on stderr for easier output filtering.
        cerr << i << " " << sizeKB << endl;
    }
}
