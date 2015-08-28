#ifndef SPLIT_INDEX_EXACT_HPP
#define SPLIT_INDEX_EXACT_HPP

#include <string>

#include "split_index_1.hpp"

class SplitIndexExact : public SplitIndex1
{
public:
    SplitIndexExact(const std::vector<std::string> &words, int minWordLength = -1);
    
    std::string toString() const override;

protected:
    // The part is a prefix.
    bool searchPartPref(const char *keyPart, size_t keySize,
                        const char *matchPart, size_t matchSize, std::string &results) override;
    // The part is a suffix.
    bool searchPartSuf(const char *keyPart, size_t keySize,
                       const char *matchPart, size_t matchSize, std::string &results) override;

};

#endif // SPLIT_INDEX_EXACT_HPP
