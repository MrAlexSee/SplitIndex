#ifndef SPLIT_INDEX_1_COMP_DOUBLE_HPP
#define SPLIT_INDEX_1_COMP_DOUBLE_HPP

#include <string>

#include "split_index_1_comp.hpp"

class SplitIndex1CompDouble : public SplitIndex1Comp
{
public:
    SplitIndex1CompDouble(const std::vector<std::string> &words, int minWordLength = -1);
    ~SplitIndex1CompDouble();

    std::string toString() const override;

protected:
    void constructQgramLists() override;
  
    size_t encode(const char *word, size_t wordSize) override;

    const int nQgrams1 = 20, nQgrams2 = 80;
    const int qgramSize1 = 3, qgramSize2 = 2;

    char *tmpBuf = nullptr;
};

#endif // SPLIT_INDEX_1_COMP_DOUBLE_HPP
