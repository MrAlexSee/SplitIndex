#ifndef SPLIT_INDEX_1_HPP
#define SPLIT_INDEX_1_HPP

#include <cmath>

#include "split_index.hpp"

class SplitIndex1 : public SplitIndex
{
public:
    SplitIndex1(const std::vector<std::string> &words, int minWordLength = -1);
    ~SplitIndex1();

    std::string toString() const override;

    void construct() override;

    std::string prettyResults(const std::string &results) const override;

protected:
    void initEntry(const std::string &word) override;
    void splitWord(const std::string &word) override;

    virtual char *createEntry(const char *wordPart, size_t partSize, bool isPartSuffix) const;
    virtual void addToEntry(char **entryPtr, const char *wordPart, size_t partSize, bool isPartSuffix) const;
    virtual void appendToEntry(char *entry, size_t oldSize, const char *wordPart, size_t partSize) const;

    virtual char *advanceByWords(char *entry, uint16_t nWords) const;
    virtual const char *advanceByWords(const char *entry, uint16_t nWords) const;

    void processQuery(const std::string &query, std::string &results) override;

    // The part is a prefix.
    virtual bool searchPartPref(const char *keyPart, size_t keySize,
                                const char *matchPart, size_t matchSize, std::string &results);
    // The part is a suffix.
    virtual bool searchPartSuf(const char *keyPart, size_t keySize,
                               const char *matchPart, size_t matchSize, std::string &results);
    
    size_t calcEntrySize(const char *entry) const override;
    virtual int calcEntryNWords(const char *entry) const;

    size_t getPartSize(size_t wordSize) const override
    {
        assert(wordSize <= maxWordSize and partSizeLUT != nullptr);
        return partSizeLUT[wordSize];
    }

    virtual std::string entryToString(const char *entry) const;

    char *part1Buf = nullptr, *part2Buf = nullptr;
    size_t part1Size = 0, part2Size = 0;

    size_t *partSizeLUT = nullptr;
    static constexpr size_t maxWordSize = 128;
};

#endif // SPLIT_INDEX_1_HPP
