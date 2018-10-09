#include <boost/format.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "split_index_1_comp_opt.hpp"
#include "../utils/math_utils.hpp"
#include "../utils/string_utils.hpp"

using namespace std;

namespace split_index
{

SplitIndex1CompOpt::SplitIndex1CompOpt(const vector<string> &words, int minWordLength)
    :SplitIndex1Comp(words, minWordLength)
{ }

SplitIndex1CompOpt::~SplitIndex1CompOpt()
{
    for (const auto buf : tmpBufVec)
    {
        delete[] buf;
    }
} 

string SplitIndex1CompOpt::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }
    
    string out = (boost::format("\nQ-gram counts: %1%") % utils::StringUtils::vecToStr(qgramCounts)).str();
    return SplitIndex1::toString() + out;
}

void SplitIndex1CompOpt::setQGramCounts(const vector<int> &counts)
{
    qgramCounts = counts;
    constructQgramListsFromCounts(qgramCounts);

    countsSet = true;
}

void SplitIndex1CompOpt::constructQgramLists()
{
    tmpBufVec.clear();

    // There is a tmp buffer for each but the last one q-gram size because the last one is encoded directly
    // to codingBuf.
    for (size_t i = 0; i < maxQgramSize - 2; ++i)
    {
        tmpBufVec.push_back(new char[maxWordSize]);
    }

    if (countsSet)
        return;

#if USE_FIXED_QGRAMS
    size_t nElems = sizeof(fixedQgramCounts) / sizeof(fixedQgramCounts[0]);
    qgramCounts = vector<int>(fixedQgramCounts, fixedQgramCounts + nElems);
#else
    // We look for the optimal combination of q-grams.
    setOptQgramFeatures();
#endif

    constructQgramListsFromCounts(qgramCounts);

    assert(qgramCounts.size() == maxQgramSize - 1);
    assert(std::accumulate(qgramCounts.begin(), qgramCounts.end(), 0) == maxNGrams);

    assert(static_cast<int>(qgramToChar.size()) == maxNGrams);
    assert(static_cast<int>(charToQgram.size()) == maxNGrams);
}

void SplitIndex1CompOpt::setOptQgramFeatures()
{
    assert(wordSet.size() > 0);
    
    auto qgramCountsList = constructQgramCands();
    cout << endl << "Constructed q-gram candidate list of size: " << qgramCountsList.size() << endl;

    vector<vector<string>> orders;

    for (int q = 2; q <= maxQgramSize; ++q)
    {
        cout << boost::format("Precalculating the orders: %1%/%2%") % q % int(maxQgramSize) << endl;
        orders.push_back(constructQGramOrder(q));
    }

    size_t minSize = 0, iBest = 0;

    for (size_t i = 0; i < qgramCountsList.size(); ++i)
    {
        size_t curSize = calcCompression(qgramCountsList[i], orders);

        cout << boost::format("Checked q-gram compression: %1%/%2% %3% (S = %4$.1f KB)")
                % (i + 1) % qgramCountsList.size()
                % utils::StringUtils::vecToStr(qgramCountsList[i]) % (curSize / 1024.0) << endl;

        if (minSize == 0 or curSize < minSize)
        {
            minSize = curSize;
            iBest = i;
        }
    }

    this->qgramCounts = qgramCountsList[iBest];

    cout << endl << boost::format("Best counts = %1%, compr. size = %2$.1f KB")
                    % utils::StringUtils::vecToStr(qgramCounts) % (minSize / 1024.0) << endl;
}

vector<vector<int>> SplitIndex1CompOpt::constructQgramCands()
{
    vector<int> range;

    for (int q = qRangeStart; q <= maxNGrams; q += qRangeStep)
    {
        range.push_back(q);
    }
    cout << boost::format("Created a range: [%1%, %2%] of size %3%")
            % int(qRangeStart) % int(maxNGrams) % range.size() << endl;

    vector<vector<int>> qgramCountsList = utils::MathUtils::countCartProd(range, maxQgramSize - 1);
    size_t cartProdSize = qgramCountsList.size();

    cout << "Created Cart. product, size = " << cartProdSize << endl;

    // We leave only the products which add up to the total count.
    int i = 1;

    for (auto it = qgramCountsList.begin() ; it != qgramCountsList.end(); )
    {
        cout << boost::format("\rFiltering: %1%/%2%") % i % cartProdSize << flush;

        int sum = std::accumulate(it->begin(), it->end(), 0);

        if (sum == maxNGrams)
        {
            ++it;
        }
        else
        {
            it = qgramCountsList.erase(it);
        }

        i += 1;
    }

    return qgramCountsList;
}

void SplitIndex1CompOpt::constructQgramListsFromCounts(const vector<int> &curQgramCounts)
{
    qgramToChar.clear();
    charToQgram.clear();

    for (size_t i = 0; i < curQgramCounts.size(); ++i)
    {
        char curFirstChar = firstChar + qgramToChar.size();
        assert(curFirstChar >= firstChar); 

        constructQGrams(curQgramCounts[i], 2 + i, curFirstChar);
    }
}

size_t SplitIndex1CompOpt::calcCompression(const vector<int> &curQgramCounts,
                                           const vector<vector<string>> &orders)
{
    qgramToChar.clear();
    charToQgram.clear();

    size_t totalSize = 0;
    char curFirstChar = this->firstChar;

    for (size_t iQ = 0; iQ < curQgramCounts.size(); ++iQ)
    {
        for (int iS = 0; iS < curQgramCounts[iQ] and iS < orders[iQ].size(); ++iS)
        {
            assert(curFirstChar < 255);

            qgramToChar[orders[iQ][iS]] = curFirstChar;
            charToQgram[curFirstChar] = orders[iQ][iS];

            curFirstChar += 1;
        }
    }

    this->qgramCounts = curQgramCounts;

    for (const auto &word : wordSet)
    {
        totalSize += encode(word.c_str(), word.size());
    }

    return totalSize;
}

// Encoding always starts from the longest q-grams.
size_t SplitIndex1CompOpt::encode(const char *word, size_t wordSize)
{
    // Otherwise rewrite the encoding below.
    assert(qgramCounts.size() >= 2 and tmpBufVec.size() >= 1);

    size_t newSize = encodeToBuf(word, wordSize, tmpBufVec.front(), qgramCounts.size() + 1);

    for (size_t i = 0; i < qgramCounts.size() - 2; ++i)
    {
        assert(i + 1 < tmpBufVec.size());
        newSize = encodeToBuf(tmpBufVec[i], newSize, this->tmpBufVec[i + 1], qgramCounts.size() - i);
    }
 
    return encodeToBuf(tmpBufVec.back(), newSize, this->codingBuf, 2);
}

#if USE_FIXED_QGRAMS
const int SplitIndex1CompOpt::fixedQgramCounts[maxQgramSize - 1] = FIXED_QGRAMS;
#endif

} // namespace split_index
