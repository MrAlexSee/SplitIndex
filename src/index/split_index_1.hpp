#ifndef SPLIT_INDEX_1_HPP
#define SPLIT_INDEX_1_HPP

#include <cmath>

#include "split_index.hpp"

namespace split_index
{

/** Split index for k = 1. */
class SplitIndex1 : public SplitIndex
{
public:
    SplitIndex1(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType);
    ~SplitIndex1() override;

    void construct() override;
    std::string toString() const override;

protected:
    void initEntry(const std::string &word) override;
    int processQuery(const std::string &query, std::string &results) override;

    size_t calcEntrySizeB(const char *entry) const override;

    /** Splits the word into two and stores the parts (prefix and suffix) in temporary buffers. */
    void storePrefixSuffixInBuffers(const std::string &word);

    /** Creates a new entry containing a [wordPart] of size [partSize].
     * Part is either a prefix or a suffix, indicated by [isPartSuffix]. */
    virtual char *createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const;

    /** Adds a [wordPart] of size [partSize] to an existing entry pointed to by [entryPtr].
     * Part is either a prefix or a suffix, indicated by [isPartSuffix]. */
    virtual void addToEntry(char **entryPtr,
        const char *wordPart, size_t partSize,
        bool isPartSuffix) const;

    /** A helper function for adding a word at the end of an existing entry. */
    virtual void appendToEntry(char *entry, 
        const char *wordPart, size_t partSize,
        size_t oldEntrySize) const;

    virtual int searchWithPrefixAsKey(std::string &results);
    virtual int searchWithSuffixAsKey(std::string &results);

    virtual char *advanceInEntryByWordCount(char *entry, uint16_t nWords) const;
    virtual const char *advanceInEntryByWordCount(const char *entry, uint16_t nWords) const;

    virtual int calcEntryNWords(const char *entry) const;

    virtual std::string entryToString(const char *entry) const;

    /** Lookup table to speed up access of prefix size.
     * There are 2 parts, i.e. a prefix and a suffix for k = 1. */
    size_t *prefixSizeLUT = nullptr;

    /** Temporarily stores the size of word prefix (1st part). */
    size_t prefixSize = 0;
    /** Temporarily stores the word prefix. */
    char *prefixBuf = nullptr;
    /** Temporarily stores the size of word suffix (2nd part). */
    size_t suffixSize = 0;
    /** Temporarily stores the word suffix. */
    char *suffixBuf = nullptr;
};

} // namespace split_index

#endif // SPLIT_INDEX_1_HPP
