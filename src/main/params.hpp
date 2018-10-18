#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

namespace split_index
{

// This struct is used in main.cpp.
struct Params
{
    /*
     *** COMMAND-LINE PARAMS
     */

    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    /** Dump input files and params info with elapsed and throughput to output file. Cmd arg -d. */
    bool dumpToFile = false;

    /** Hash type used by the split index. */
    std::string hashType;

    /** Split index type. */
    std::string indexType;

    /** Number of iterations per pattern lookup. */
    int nIter;

    /** Maximum load factor which causes rehashing when crossed. */
    float maxLoadFactor;

    /** Minumum word length in the input dictionary (shorter words are ignored). */
    int minWordLength;

    /** Input data (dictionary and patterns) separator. */
    std::string separator = "\n";

    /** Input dictionary file path (positional arg 1). */
    std::string inDictFile;
    /** Input pattern file path (positional arg 2). */
    std::string inPatternFile;

    /** Output file path. Cmd arg -o. */
    std::string outFile;

    /*
     *** CONSTANTS
     */

    /** Returned from main on failure. */
    static constexpr int errorExitCode = 1;

    /** Current version: major.minor.patch */
    const std::string versionInfo = "split_index v0.1.0";
    const std::string usageInfoString = "[options] <input dictionary file> <input pattern file>";
};

} // namespace split_index

#endif // PARAMS_HPP
