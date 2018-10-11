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
#include <unordered_set>
#include <vector>

#include "../index/split_index_factory.hpp"
#include "../utils/file_io.hpp"
#include "../utils/string_utils.hpp"

#include "params.hpp"

using namespace split_index;
using namespace split_index::hash_functions;
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

void initSplitIndexParams(hash_functions::HashFunctions::HashType &hashType,
    SplitIndexFactory::IndexType &indexType);
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
        cerr << "Cannot access input dictionary file (doesn't exist or insufficient permissions): " << 
            params.inDictFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    if (utils::FileIO::isFileReadable(params.inPatternFile) == false)
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << 
            params.inPatternFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    cout << "All input files exist" << endl;
    return true;
}

int run()
{
    try
    {
        vector<string> dict = utils::FileIO::readWords(params.inDictFile, params.separator);
        vector<string> queries = utils::FileIO::readWords(params.inPatternFile, params.separator);
       
        utils::StringUtils::filterWords(dict, params.minWordLength);
        utils::StringUtils::filterWords(queries, params.minWordLength);

        runSearch(dict, queries);
    }
    catch (const exception &ex)
    {
        cerr << endl << "Fatal error occurred: " << ex.what() << endl;
        return params.errorExitCode;
    }

    return 0;
}

void runSearch(const vector<string> &dict, const vector<string> &queries)
{
    HashFunctions::HashType hashType;
    SplitIndexFactory::IndexType indexType;

    initSplitIndexParams(hashType, indexType);
    unordered_set<string> wordSet(dict.begin(), dict.end());

    cout << endl << boost::format("Processing #words (dict) = %1%, #queries = %2%")
            % wordSet.size() % queries.size() << endl;

    SplitIndex *index = SplitIndexFactory::initIndex(wordSet, hashType, indexType);

    cout << "Index constructed:" << endl;
    cout << index->toString() << endl;

    string results;

    const int nMatches = index->search(queries, results, params.nIter);
    cout << "#matches = " << nMatches << endl;

    const float elapsedTotalUs = index->getElapsedUs();
    const float elapsedPerIterUs = elapsedTotalUs / static_cast<float>(params.nIter);

    cout << boost::format("Elapsed: %1% us") % elapsedPerIterUs << endl;
    delete index;
}

void initSplitIndexParams(hash_functions::HashFunctions::HashType &hashType,
    SplitIndexFactory::IndexType &indexType)
{
    using namespace hash_functions;

    const map<string, HashFunctions::HashType> hashTypeMap {
        { "city", HashFunctions::HashType::City },
        { "farm", HashFunctions::HashType::Farm },
        { "farsh", HashFunctions::HashType::Farsh },
        { "fnv1", HashFunctions::HashType::FNV1 },
        { "fnv1a", HashFunctions::HashType::FNV1a },
        { "murmur3", HashFunctions::HashType::Murmur3 },
        { "sdbm", HashFunctions::HashType::Sdbm },
        { "spookyv2", HashFunctions::HashType::SpookyV2 },
        { "superfast", HashFunctions::HashType::SuperFast },
        { "xxhash", HashFunctions::HashType::XxHash }
    };

    if (hashTypeMap.count(params.hashType) == 0)
    {
        throw runtime_error("bad hash type: " + params.hashType);
    }

    hashType = hashTypeMap.at(params.hashType);

    const map<string, SplitIndexFactory::IndexType> indexTypeMap {
        { "k1", SplitIndexFactory::IndexType::K1 },
        { "k1comp", SplitIndexFactory::IndexType::K1Comp }
    };

    if (indexTypeMap.count(params.indexType) == 0)
    {
        throw runtime_error("bad index type: " + params.indexType);
    }

    indexType = indexTypeMap.at(params.indexType);

    cout << boost::format("Using index type = %1%, hash function = %2%")
        % params.indexType % params.hashType << endl;
}

} // namespace split_index
