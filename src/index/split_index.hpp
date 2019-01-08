#ifndef SPLIT_INDEX_HPP
#define SPLIT_INDEX_HPP

#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "../hash_function/hash_functions.hpp"
#include "../hash_map/hash_map.hpp"

namespace split_index
{

/** Abstract base split index class. */
class SplitIndex
{
public:
    /** Defines the type containing all matches reported by the split index. */
    using ResultSetType = std::unordered_set<std::string>;

    SplitIndex(const std::unordered_set<std::string> &wordSetArg);
    virtual ~SplitIndex();

    virtual void construct();
    virtual std::string toString() const;

    /** Performs a search for [queries] and returns the set of matching words, iterates [nIter] times. */
    ResultSetType search(const std::vector<std::string> &queries, int nIter = 1);

    /** Performs a search for [queries] and returns the set of matching words.
     * The number of matches for each query is dumped to standard output. Time measurement is not performed. */
    ResultSetType searchAndDumpMatchCounts(const std::vector<std::string> &queries);

    /** Returns the total size of stored words in bytes. */
    long calcWordsSizeB() const;
    /* Returns the size of the underlying hash map in bytes. */
    long calcHashMapSizeB() const { return hashMap->calcTotalSizeB(); }

    /** Returns the time elapsed during the search in microseconds (us). */
    float getElapsedUs() const { return elapsedUs; }

protected:
    virtual void initEntry(const std::string &word) = 0;

    /** Processes a query, adding matches to [results]. */
    virtual void processQuery(const std::string &query, ResultSetType &results) = 0;

    /** Returns the size of an entry in bytes, including the terminating '\0' if present. */
    virtual size_t calcEntrySizeB(const char *entry) const = 0;

    /** True if index has been constructed, false otherwise. */
    bool constructed = false;

    /** Elapsed time during the search in microseconds. */
    float elapsedUs = 0.0f;

    hash_map::HashMap *hashMap = nullptr;
    std::unordered_set<std::string> wordSet;

    /** The number of words is multiplied by this factor and passed as a bucket count hint to the hash map. */
    static constexpr float nBucketsHintFactor = 0.1;
    /** Maximum word size, set to 127 because we use 8-bit counters.
     * Can be set to 255 if using unsigned char when compiling. */
    static constexpr size_t maxWordSize = 127;
};

} // namespace split_index

#endif // SPLIT_INDEX_HPP
