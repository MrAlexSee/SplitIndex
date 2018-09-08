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

}

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
       ("help,h", "display help message")
       ("in-dict-file,i", po::value<string>(&params.inDictFile)->required(), "input dictionary file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
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
    SplitIndex *index = SplitIndexFactory::initIndex(words, params.indexType, params.minWordLength);
    cout << "Index constructed" << endl;

    string results;
    int nMatches = index->search(queries, results);

    cout << "#matches = " << nMatches << endl;

    float elapsedTotalUs = index->getElapsedUs();
    float elapsedPerIterUs = elapsedTotalUs / static_cast<float>(params.nIter);

    cout << boost::format("Elapsed: %1% us") % elapsedPerIterUs << endl;
    delete index;
}

} // namespace split_index
