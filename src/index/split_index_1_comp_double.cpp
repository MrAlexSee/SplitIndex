#include <boost/format.hpp>
#include <cstring>
#include <iostream>
#include <utility>

#include "split_index_1_comp_double.hpp"

using namespace std;

namespace split_index
{

SplitIndex1CompDouble::SplitIndex1CompDouble(const vector<string> &words, int minWordLength)
    :SplitIndex1Comp(words, minWordLength)
{
    tmpBuf = new char[maxWordSize];
}

SplitIndex1CompDouble::~SplitIndex1CompDouble()
{
    delete[] tmpBuf;
} 

string SplitIndex1CompDouble::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }
    
    string out = (boost::format("\n%1% %2%-grams; %3% %4%-grams")
                  % nQgrams1 % qgramSize1 % nQgrams2 % qgramSize2).str();

    return SplitIndex1::toString() + out;
}

void SplitIndex1CompDouble::constructQgramLists()
{
    qgramToChar.clear();
    charToQgram.clear();

    constructQGrams(nQgrams1, qgramSize1, firstChar);

    assert(qgramToChar.size() == nQgrams1);
    assert(charToQgram.size() == nQgrams1);

    constructQGrams(nQgrams2, qgramSize2, firstChar + nQgrams1);

    assert(qgramToChar.size() == nQgrams1 + nQgrams2);
    assert(charToQgram.size() == nQgrams1 + nQgrams2);
}
  
size_t SplitIndex1CompDouble::encode(const char *word, size_t wordSize)
{
    size_t newSize = encodeToBuf(word, wordSize, this->tmpBuf, this->qgramSize1);
    return encodeToBuf(this->tmpBuf, newSize, this->codingBuf, this->qgramSize2);
}

} // namespace split_index
