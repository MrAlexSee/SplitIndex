#ifndef SPLIT_INDEX_1_COMP_OPT_HPP
#define SPLIT_INDEX_1_COMP_OPT_HPP

#include <string>
#include <vector>

#include "split_index_1_comp.hpp"

#define USE_FIXED_QGRAMS false
#define FIXED_QGRAMS { 16, 66, 18 }

namespace split_index
{

class SplitIndex1CompOpt : public SplitIndex1Comp
{
public:
    SplitIndex1CompOpt(const std::vector<std::string> &words, int minWordLength = -1);
    ~SplitIndex1CompOpt();

    std::string toString() const override;

    void setQGramCounts(const std::vector<int> &counts);

protected:
    void constructQgramLists() override;
    void constructForAllQgrams();

    void setOptQgramFeatures();
    std::vector<std::vector<int>> constructQgramCands();

    void constructQgramListsFromCounts(const std::vector<int> &curQgramCounts);
  
    size_t calcCompression(const std::vector<int> &curQgramCounts,
                           const std::vector<std::vector<std::string>> &orders);

    size_t encode(const char *word, size_t wordSize) override;

    // If false, do not calculate the optimal counts when being constructed.
    bool countsSet = false;

    // Longest q-grams first (because they are encoded first).
    std::vector<int> qgramCounts; // 2, 3, ..., maxQgramSize
    std::vector<char *> tmpBufVec;

    // Considered q-grams are in the range [2, maxQgramSize].
    static constexpr int maxQgramSize = 4;

    // Make sure that ranges can add up to maxNGrams.
    static constexpr int qRangeStart = 0;
    static constexpr int qRangeStep = 2;
    static constexpr int maxNGrams = 100;

#if USE_FIXED_QGRAMS
    // This is useful if q-grams are precalculated.
    // Make sure that these sum up to maxNGrams.
    static const int fixedQgramCounts[maxQgramSize - 1]; // Initialized in *.cpp.
#endif
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_OPT_HPP
