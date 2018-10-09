#ifndef SPLIT_INDEX_1_COMP_EXT_HPP
#define SPLIT_INDEX_1_COMP_EXT_HPP

#include "split_index_1_comp.hpp"

namespace split_index
{

class SplitIndex1CompExt : public SplitIndex1Comp
{
public:
    SplitIndex1CompExt(const std::vector<std::string> &words, int minWordLength = -1);
    std::string toString() const override;

protected:
    void initEntry(const std::string &word) override;

    virtual char *createEntryExt(const char *wordPart, size_t partSize,
                                 size_t decSize, bool isPartSuffix) const;
    virtual void addToEntryExt(char **entryPtr, const char *wordPart, size_t partSize,
                               size_t decSize, bool isPartSuffix) const;
    virtual void appendToEntryExt(char *entry, size_t oldSize, const char *wordPart,
                                  size_t partSize, size_t decSize) const;

    // The part is a prefix.
    bool searchPartPref(const char *keyPart, size_t keySize,
                        const char *matchPart, size_t matchSize, std::string &results) override;
    // The part is a suffix.
    bool searchPartSuf(const char *keyPart, size_t keySize,
                       const char *matchPart, size_t matchSize, std::string &results) override;

    char *advanceByWords(char *entry, uint16_t nWords) const override;
    const char *advanceByWords(const char *entry, uint16_t nWords) const override;

    size_t calcEntrySize(const char *entry) const override;
    int calcEntryNWords(const char *entry) const override;

    std::string entryToString(const char *entry) const override;
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_EXT_HPP
