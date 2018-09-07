#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

// k = 1                               -> 0
// k = 1, compression                  -> 1
// k = 1, compression w. length info   -> 2
// k = 1, compression, doubled q-grams -> 3
// k = 1, compression, opt. q-grams    -> 4
// k = 1, using generic split_index_k  -> 5
// k = 2                               -> 6
// k = 3                               -> 7
// Exact match (based on k = 1)        -> 8
#define INDEX_TYPE 0

// City      -> 0
// FNV1      -> 1
// FNV1a     -> 2
// Murmur3   -> 3
// sdbm      -> 4
// SpookyV2  -> 5
// SuperFast -> 6
// xxHash    -> 7
// farsh     -> 8
// FarmH     -> 9
#define HASH_TYPE 7

// 0 -> English
// 1 -> DNA
#define QUERY_ALPHABET 0

#define USE_DEBUG_PRINT 0
#define USE_VERBOSE_DEBUG_PRINT 0

// Constructs the index for all different q-gram combinations in order to check its size.
// The queries are not run.
#define CHECK_ALL_QGRAMS false

// This struct is used by main/main.cpp.
struct Params
{
    // Minumum word length in the input dictionary (shorter words are ignored).
    const int minWordLength = 4;

    // Number of iterations, useful only for time measurements (averages the elapsed time).
    const int nIterations = 1000;

    // Number of generated queries (used if no input file with queries is provided).
    const int nQueries = 5000;

    // Symbols used for generating the queries.
#if QUERY_ALPHABET == 0
    const std::string queryAlphabet = "abcdefghijklmnopqrstuvwxyz'";
#elif QUERY_ALPHABET == 1
    const std::string queryAlphabet = "acgnt";
#else
    #error Bad QUERY_ALPHABET
#endif

    // Max #errors for generated queries (each error appears with 50% probability).
    const int maxNErrors = 3;
    const std::string outFilePath = "search_ham.dat";

    /** Input dictionary file path (positional arg 1). */
    std::string inDictFile = "";
    /** Input pattern file path (positional arg 2). */
    std::string inPatternFile = "";

    /*
     *** CONSTANTS
     */

    /** Returned from main on failure. */
    static constexpr int errorExitCode = 1;

    /** Current version: major.minor.patch */
    const std::string versionInfo = "split_index v0.1.0";
    const std::string usageInfoString = "[options] <input dictionary file> <input pattern file>";
};

#endif // PARAMS_HPP
