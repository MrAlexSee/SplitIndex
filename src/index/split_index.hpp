#ifndef SPLIT_INDEX_HPP
#define SPLIT_INDEX_HPP

#include <string>
#include <unordered_set>
#include <vector>

#include "../hash_map/hash_map.hpp"
#include "../main/params.hpp"

class SplitIndex
{
public:
    // Use -1 to ignore the minimum word length.
    SplitIndex(const std::vector<std::string> &words, int minWordLength = -1);
    virtual ~SplitIndex();

    virtual std::string toString() const;
    virtual void construct();

    // Returns the elapsed time in microseconds (us).
    long long search(const std::vector<std::string> &queries, std::string &results);

    long getWordsSizeB() const;
    virtual std::string prettyResults(const std::string &results) const = 0;

    long getTotalSizeB() const { return map->calcTotalSizeB(); }

    float getElapsedUs() const { return 0.0; }

protected:
    void initMap();

    virtual void initEntry(const std::string &word) = 0;
    virtual void splitWord(const std::string &word) = 0;

    virtual void processQuery(const std::string &query, std::string &results) = 0;

    static void addResult(const char *str, size_t size,
                          std::string &results, char iPart);

    // The returned size should include the terminating '\0'.
    virtual size_t calcEntrySize(const char *entry) const = 0;

    virtual size_t getPartSize(size_t wordSize) const = 0;

    // The old size should include the terminating '\0'.
    static void moveToRight(char *str, size_t nPlaces, size_t size);

    bool constructed = false;

    std::unordered_set<std::string> wordSet;

    int minWordLength;
    split_index::hash_map::HashMap *map = nullptr;

    // The number of words is multiplied by this factor and fed as a size hint to the hash map.
    static constexpr double sizeHintFactor = 0.05;

    static constexpr double maxLoadFactor = 2.0;
    static constexpr size_t maxWordSize = 255; // We use 8-bit (char) counters.
};

#endif // SPLIT_INDEX_HPP
