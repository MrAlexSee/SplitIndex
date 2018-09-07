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

namespace po = boost::program_options;

namespace split_index
{

namespace
{

Params params;

/** Indicates that program execution should continue after checking parameters. */
constexpr int paramsResContinue = -1;

}

/** Handles cmd-line parameters, returns -1 if program execution should continue. */
int handleParams(int argc, const char **argv);
/** Returns true if input files are readable, false otherwise. */
bool checkInputFiles(const char *execName);
/** Runs the main program and returns the program exit code. */
int run();

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
    catch (const po::error& e)
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
    // if (Helpers::isFileReadable(params.inDictFile) == false)
    // {
    //     cerr << "Cannot access input dictionary file (doesn't exist or insufficient permissions): " << params.inDictFile << endl;
    //     cerr << "Run " << execName << " -h for more information" << endl << endl;

    //     return false;
    // }

    // if (Helpers::isFileReadable(params.inPatternFile) == false)
    // {
    //     cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
    //     cerr << "Run " << execName << " -h for more information" << endl << endl;

    //     return false;
    // }

    return true;
}

int run()
{
    // try
    // {
    //     vector<string> dict = Helpers::readWords(params.inDictFile, params.separator);
    //     vector<string> patterns = Helpers::readWords(params.inPatternFile, params.separator);
       
    //     filterInput(dict, patterns);

    //     cout << "=====" << endl;
    //     cout << boost::format("Read #words = %1%, #queries = %2%") % dict.size() % patterns.size() << endl;
     
    //     runFingerprints(dict, patterns);
    // }
    // catch (const exception &e)
    // {
    //     cerr << endl << "Fatal error occurred: " << e.what() << endl;
    //     return params.errorExitCode;
    // }

    return 0;
}

} // namespace split_index
