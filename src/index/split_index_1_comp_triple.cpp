#include <algorithm>
#include <boost/format.hpp>
#include <cassert>
#include <cstring>
#include <iostream>

#include "split_index_1_comp_triple.hpp"

using namespace std;

namespace split_index
{

SplitIndex1CompTriple::SplitIndex1CompTriple(const unordered_set<string> &wordSet,
    hash_functions::HashFunctions::HashType hashType,
    float maxLoadFactor)
        :SplitIndex1Comp(wordSet, hashType, maxLoadFactor)
{
    encodingTmpBuf1 = new char[maxWordSize];
    encodingTmpBuf2 = new char[maxWordSize];
}

SplitIndex1CompTriple::~SplitIndex1CompTriple()
{
    delete[] encodingTmpBuf1;
    delete[] encodingTmpBuf2;
}

string SplitIndex1CompTriple::toString() const
{
    if (not constructed)
    {
        return "Index not constructed";
    }

    string out = "\nWith compression:\n";
    out += (boost::format("#2-grams = %1%, #3-grams = %2%, #4-grams = %3%")
        % curN2grams % curN3grams % curN4grams).str();

    return SplitIndex1::toString() + out;
}

void SplitIndex1CompTriple::calcQgramsAndFillMaps()
{
    qgramToChar.clear();
    charToQgram.clear();

    char curFirstChar = firstChar;

    vector<string> q2grams = calcQGramsOrderedByFrequency(n2grams, 2);
    curN2grams = q2grams.size();

    fillQgramMaps(q2grams, curFirstChar); // Invalidates insides of the qgrams vector.
    curFirstChar += curN2grams;
    
    vector<string> q3grams = calcQGramsOrderedByFrequency(n3grams, 3);
    curN3grams = q3grams.size();

    fillQgramMaps(q3grams, curFirstChar); // Invalidates insides of the qgrams vector.
    curFirstChar += curN3grams;

    vector<string> q4grams = calcQGramsOrderedByFrequency(n4grams, 4);
    curN4grams = q4grams.size();

    fillQgramMaps(q4grams, curFirstChar); // Invalidates insides of the qgrams vector.

    assert(qgramToChar.size() == curN2grams + curN3grams + curN4grams);
    assert(charToQgram.size() == curN2grams + curN3grams + curN4grams);
}

size_t SplitIndex1CompTriple::encodeToBuf(const char *word, size_t wordSize)
{        
    // Note that the chars which are used for encoding do not appear in the input text,
    // so such sequential encoding should not run into any unintended q-gram overlap issues.

    const char *curSrc = word;
    size_t curWordSize = wordSize;

    // We start with the longest q-grams.
    if (curWordSize >= 4)
    {
        curWordSize = encodeToBuf(encodingTmpBuf1, word, wordSize, 4);
        curSrc = encodingTmpBuf1;
    }

    if (curWordSize >= 3)
    {
        curWordSize = encodeToBuf(encodingTmpBuf2, curSrc, curWordSize, 3);
        curSrc = encodingTmpBuf2;
    }

    if (curWordSize >= 2)
    {
        return encodeToBuf(codingBuf, curSrc, curWordSize, 2);
    }
    else
    {
        memcpy(codingBuf, curSrc, curWordSize);
        return curWordSize;
    }
}

size_t SplitIndex1CompTriple::encodeToBuf(char *dstBuf, const char *word, size_t curWordSize, size_t curQgramSize)
{
    assert(curQgramSize <= curWordSize);

    const size_t qgramEnd = curWordSize - curQgramSize + 1;
    size_t iBuf = 0;

    for (size_t iW = 0; iW < curWordSize; ++iW)
    {
        if (iW < qgramEnd)
        {
            assert(iW + curQgramSize <= curWordSize);
            auto it = qgramToChar.find(string(word + iW, curQgramSize));

            if (it != qgramToChar.end())
            {
                dstBuf[iBuf++] = it->second;
                iW += curQgramSize - 1;
            }
            else
            {
                dstBuf[iBuf++] = word[iW];
            }
        }
        else
        {
            dstBuf[iBuf++] = word[iW];
        }
    }

    assert(iBuf < maxWordSize);
    return iBuf;
}

} // namespace split_index
