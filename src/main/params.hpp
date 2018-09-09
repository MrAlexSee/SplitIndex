#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

namespace split_index
{

// This struct is used by main/main.cpp.
struct Params
{
    /*
     *** COMMAND-LINE PARAMS
     */

    // These parameters are set by handleParams() in main.cpp after parsing command line args.

    /** Hash type used by the split index. */
    std::string hashType;

    /** Split index type. */
    std::string indexType = 0;

    /** Number of iterations per pattern lookup. */
    int nIter;

    /** Minumum word length in the input dictionary (shorter words are ignored). */
    int minWordLength;

    /** Input data (dictionary and patterns) separator. */
    std::string separator = "\n";

    /** Input dictionary file path (positional arg 1). */
    std::string inDictFile;
    /** Input pattern file path (positional arg 2). */
    std::string inPatternFile;

    /** Output file path. */
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
