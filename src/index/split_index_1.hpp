#ifndef SPLIT_INDEX_1_HPP
#define SPLIT_INDEX_1_HPP

#include <cmath>
#include <set>

#include "split_index.hpp"

#ifndef SPLIT_INDEX_1_WHITEBOX
#define SPLIT_INDEX_1_WHITEBOX
#endif

namespace split_index
{

/** Split index for k = 1. */
class SplitIndex1 : public SplitIndex
{
public:
    SplitIndex1(const std::unordered_set<std::string> &wordSet,
        hash_functions::HashFunctions::HashType hashType, float maxLoadFactor);
    ~SplitIndex1() override;

    void construct() override;
    std::string toString() const override;

protected:
    void initEntry(const std::string &word) override;
    void processQuery(const std::string &query, ResultSetType &results) override;

    size_t calcEntrySizeB(const char *entry) const override;

    size_t getMinWordSize() const override { return 2; }

    /** Returns the number of words (word parts) stored in [entry]. */
    static size_t calcEntryNWords(const char *entry);

    /** Splits [word] into two and stores the parts (prefix and suffix) in prefixBuf and suffixBuf, resp. */
    void storePrefixSuffixInBuffers(const std::string &word);

    /** Creates a new entry containing a [wordPart] of size [partSize].
     * Part is either a prefix or a suffix, indicated by [isPartSuffix]. */
    virtual char *createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const;

    /** Adds a [wordPart] of size [partSize] to an existing entry pointed to by [entryPtr].
     * Part is either a prefix or a suffix, indicated by [isPartSuffix]. */
    virtual void addToEntry(char **entryPtr,
        const char *wordPart, size_t partSize,
        bool isPartSuffix) const;

    /** A helper function for adding a [wordPart] of size [partSize],
     * at the end of an existing [entry] having [oldEntrySize] bytes . */
    virtual void appendToEntry(char *entry, size_t oldEntrySize,
        const char *wordPart, size_t partSize) const;

    virtual void searchWithPrefixAsKey(ResultSetType &results);
    virtual void searchWithSuffixAsKey(ResultSetType &results);

    /** Returns a pointer pointing [nWords] further within the [entry],
     * which must point towards a word size byte. */
    virtual char *advanceInEntryByWordCount(char *entry, uint16_t nWords) const;
    virtual const char *advanceInEntryByWordCount(const char *entry, uint16_t nWords) const;

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

    SPLIT_INDEX_1_WHITEBOX
};

} // namespace split_index

#endif // SPLIT_INDEX_1_HPP
