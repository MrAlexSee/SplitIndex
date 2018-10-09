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
    void splitWord(const std::string &word);

    int processQuery(const std::string &query, std::string &results) override;

    size_t calcEntrySizeB(const char *entry) const override;
    inline size_t getPartSize(size_t wordSize) const override;

    virtual char *createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const;
    virtual void addToEntry(char **entryPtr, const char *wordPart, size_t partSize, bool isPartSuffix) const;
    virtual void appendToEntry(char *entry, size_t oldSize, const char *wordPart, size_t partSize) const;

    virtual char *advanceByWords(char *entry, uint16_t nWords) const;
    virtual const char *advanceByWords(const char *entry, uint16_t nWords) const;

    // The part is a prefix.
    virtual bool searchPartPref(const char *keyPart, size_t keySize,
                                const char *matchPart, size_t matchSize, std::string &results);
    // The part is a suffix.
    virtual bool searchPartSuf(const char *keyPart, size_t keySize,
                               const char *matchPart, size_t matchSize, std::string &results);
    
    virtual int calcEntryNWords(const char *entry) const;

    virtual std::string entryToString(const char *entry) const;

    /** Lookup table to speed up access of the 1st word part size (there are 2 parts for k = 1). */
    size_t *partSizeLUT = nullptr;

    /** Temporarily stores the size of the 1st part of the word. */
    size_t part1Size = 0;
    /** Temporarily stores the 1st part of the word. */
    char *part1Buf = nullptr;
    /** Temporarily stores the size of the 2nd part of the word. */
    size_t part2Size = 0;
    /** Temporarily stores the 2nd part of the word. */
    char *part2Buf = nullptr;
};

} // namespace split_index

#endif // SPLIT_INDEX_1_HPP
