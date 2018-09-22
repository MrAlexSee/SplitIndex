/*
 *** A practical index for approximate dictionary matching with few mismatches.
 *** Authors: Aleksander Cisłak, Szymon Grabowski. License: GNU LGPL v3.
 *** Set BOOST_DIR in makefile.inc and type "make" for optimized compile.
 */

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../index/split_index_factory.hpp"
#include "../utils/file_io.hpp"

#include "params.hpp"

using namespace split_index;
using namespace std;

namespace po = boost::program_options;

namespace split_index
{

namespace
{

Params params;

/** Indicates that program execution should continue after checking parameters. */
constexpr int paramsResContinue = -1;

}

/** Handles cmd-line parameters, returns paramsResContinue if program execution should continue. */
int handleParams(int argc, const char **argv);
/** Returns true if input files are readable, false otherwise. */
bool checkInputFiles(const char *execName);
/** Runs the main program and returns the program exit code. */
int run();

/** Searches for [queries] in [words] using a split index. */
void runSearch(const vector<string> &words, const vector<string> &queries);

void initSplitIndexParams(SplitIndexFactory::HashType &hashType, SplitIndexFactory::IndexType &indexType);

} // namespace split_index

int main(int argc, const char **argv)
{
    int paramsRes = handleParams(argc, argv);
    if (paramsRes != paramsResContinue)
    {
        return paramsRes;
    }

    if (checkInputFiles(argv[0]) == false)
    {
        return params.errorExitCode;
    }

    return run();
}

namespace split_index
{

int handleParams(int argc, const char **argv)
{
    po::options_description options("Parameters");
    options.add_options()
       ("hash-type", po::value<string>(&params.hashType)->default_value("xxhash"), "hash type used by the split index: city, farm, farsh, fnv1, fnv1a, murmur3, sdbm, spookyv2, superfast, xxhash")
       ("help,h", "display help message")
       ("index-type", po::value<string>(&params.indexType)->default_value("k1"), "split index type: k1 (k = 1), k1comp (k = 1 with compression)")
       ("in-dict-file,i", po::value<string>(&params.inDictFile)->required(), "input dictionary file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("iter", po::value<int>(&params.nIter)->default_value(1), "number of iterations per pattern lookup")
       ("min-word-length", po::value<int>(&params.minWordLength)->default_value(4), "minimum word length from the input dictionary (shorter words are ignored)")
       ("out-file,o", po::value<string>(&params.outFile)->default_value("res.txt"), "output file path")
       // Not using a default value from Boost for separator because it literally prints a newline.
       ("separator,s", po::value<string>(&params.separator), "input data (dictionary and patterns) separator (default = newline)")
       ("version,v", "display version info");

    po::positional_options_description positionalOptions;

    positionalOptions.add("in-dict-file", 1);
    positionalOptions.add("in-pattern-file", 1);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                  options(options).
                  positional(positionalOptions).run(), vm);

        if (vm.count("help"))
        {
            cout << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
            cout << options << endl;

            return 0;
        }
        if (vm.count("version"))
        {
            cout << params.versionInfo << endl;
            return 0;
        }

        po::notify(vm);
    }
    catch (const po::error &e)
    {
        cerr << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
        cerr << options << endl;

        cerr << "Error: " << e.what() << endl;
        return params.errorExitCode;
    }

    return paramsResContinue;
}

bool checkInputFiles(const char *execName)
{
    if (utils::FileIO::isFileReadable(params.inDictFile) == false)
    {
        cerr << "Cannot access input dictionary file (doesn't exist or insufficient permissions): " << params.inDictFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    if (utils::FileIO::isFileReadable(params.inPatternFile) == false)
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    return true;
}

int run()
{
    try
    {
        vector<string> dict = utils::FileIO::readWords(params.inDictFile, params.separator);
        vector<string> patterns = utils::FileIO::readWords(params.inPatternFile, params.separator);
       
        cout << boost::format("Read #words = %1%, #queries = %2%") % dict.size() % patterns.size() << endl;
        runSearch(dict, patterns);
    }
    catch (const exception &e)
    {
        cerr << endl << "Fatal error occurred: " << e.what() << endl;
        return params.errorExitCode;
    }

    return 0;
}

void runSearch(const vector<string> &words, const vector<string> &queries)
{
    SplitIndexFactory::HashType hashType;
    SplitIndexFactory::IndexType indexType;

    initSplitIndexParams(hashType, indexType);

    SplitIndex *index = SplitIndexFactory::initIndex(words, hashType, indexType, params.minWordLength);
    cout << "Index constructed" << endl;

    string results;
    int nMatches = index->search(queries, results);

    cout << "#matches = " << nMatches << endl;

    float elapsedTotalUs = index->getElapsedUs();
    float elapsedPerIterUs = elapsedTotalUs / static_cast<float>(params.nIter);

    cout << boost::format("Elapsed: %1% us") % elapsedPerIterUs << endl;
    delete index;
}

void initSplitIndexParams(SplitIndexFactory::HashType &hashType, SplitIndexFactory::IndexType &indexType)
{
    map<string, SplitIndexFactory::HashType> hashTypeMap {
        { "city", SplitIndexFactory::HashType::City },
        { "farm", SplitIndexFactory::HashType::Farm },
        { "farsh", SplitIndexFactory::HashType::Farsh },
        { "fnv1", SplitIndexFactory::HashType::FNV1 },
        { "fnv1a", SplitIndexFactory::HashType::FNV1a },
        { "murmur3", SplitIndexFactory::HashType::Murmur3 },
        { "sdbm", SplitIndexFactory::HashType::Sdbm },
        { "spookyv2", SplitIndexFactory::HashType::SpookyV2 },
        { "superfast", SplitIndexFactory::HashType::SuperFast },
        { "xxhash", SplitIndexFactory::HashType::XxHash }
    };

    if (hashTypeMap.count(params.hashType) == 0)
    {
        throw invalid_argument("bad hash type: " + params.hashType);
    }

    hashType = hashTypeMap[params.hashType];

    map<string, SplitIndexFactory::IndexType> indexTypeMap { 
        { "k1", SplitIndexFactory::IndexType::K1 },
        { "k1comp", SplitIndexFactory::IndexType::K1Comp }
    };

    if (indexTypeMap.count(params.indexType) == 0)
    {
        throw invalid_argument("bad index type: " + params.indexType);
    }

    indexType = indexTypeMap[params.indexType];
}

} // namespace split_index
