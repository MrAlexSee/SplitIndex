#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "split_index_1_comp_whitebox.hpp"

#include "../src/index/split_index_1_comp.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;

}

TEST_CASE("does split index 1 comp throw for empty words", "[split_index_1_comp]")
{
    REQUIRE_THROWS(SplitIndex1Comp({ }, hashType, 1.0f));
}

TEST_CASE("is calculating q-grams ordered by frequency correct", "[split_index_1_comp]")
{
    const unordered_set<string> wordSet{ "ala", "dla", "kota", "jarek", "lubi", "psy" };    

    SplitIndex1Comp index1(wordSet, hashType, 1.0f);
    vector<string> qgrams = SplitIndex1CompWhitebox::calcQGramsOrderedByFrequency(index1, 100, 2);

    const vector<string> expected { "al", "la", "dl", "ko", "ot", "ta", "ja", "ar", "re", "ek", "lu",
        "ub", "bi", "ps", "sy" };

    REQUIRE(qgrams.size() == expected.size());

    for (const string &qgram : expected)
    {
        REQUIRE(find(qgrams.begin(), qgrams.end(), qgram) != qgrams.end());
    }

    // "la" occurs twice so it should be the first one.
    REQUIRE(qgrams.front() == "la");

    vector<string> qgrams2 = SplitIndex1CompWhitebox::calcQGramsOrderedByFrequency(index1, 5, 2);

    REQUIRE(qgrams2.size() == 5);
    REQUIRE(qgrams2.front() == "la");
}

TEST_CASE("is filling q-gram maps correct", "[split_index_1_comp]")
{
    const unordered_set<string> wordSet{ "ala", "dla", "kota", "jarek", "lubi", "psy" };    

    SplitIndex1Comp index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // This assumes di-grams (2-grams, q-gram size = 2) and 
    // that the compile-time number of q-grams is >= than the number of q-grams extracted from words.
    const vector<string> expected { "al", "la", "dl", "ko", "ot", "ta", "ja", "ar", "re", "ek", "lu",
        "ub", "bi", "ps", "sy" };

    const map<string, char> qgramToChar = SplitIndex1CompWhitebox::getQGramToCharMap(index1);
    REQUIRE(qgramToChar.size() == expected.size());

    for (const string &qgram : expected)
    {
        REQUIRE(qgramToChar.find(qgram) != qgramToChar.end());
    }    

    for (const string &qgramOut : { "aa", "cc", "ala", "kota" })
    {
        REQUIRE(qgramToChar.find(qgramOut) == qgramToChar.end());
    }

    const map<char, string> charToQgram = SplitIndex1CompWhitebox::getCharToQgramMap(index1);
    REQUIRE(qgramToChar.size() == expected.size());

    for (const auto &kv : charToQgram)
    {
        REQUIRE(find(expected.begin(), expected.end(), kv.second) != expected.end());
    }
}

TEST_CASE("is encoding to buffer correct", "[split_index_1_comp]")
{
    const unordered_set<string> wordSet{ "ala", "ma", "kota" };

    SplitIndex1Comp index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // 2-grams used for encoding: al, la, ma, ko, ot, ta.

    size_t size1 = SplitIndex1CompWhitebox::encodeToBuf(index1, "alddd", 5);
    char encoded = SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("al");

    const string expected = string(1, encoded) + "ddd";

    REQUIRE(size1 == expected.size());
    REQUIRE(memcmp(SplitIndex1CompWhitebox::getCodingBuf(index1), expected.c_str(), expected.size()) == 0);
}

TEST_CASE("is decoding to buffer correct", "[split_index_1_comp]")
{
    const unordered_set<string> wordSet{ "ala", "ma", "kota" };
 
    SplitIndex1Comp index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // 2-grams used for encoding: al, la, ma, ko, ot, ta.
    char encoded = SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("al");
    const string toDecode = string(1, encoded) + "ddd";

    size_t size1 = SplitIndex1CompWhitebox::decodeToBuf(index1, toDecode.c_str(), toDecode.size(), 10);

    REQUIRE(size1 == 5);
    REQUIRE(memcmp(SplitIndex1CompWhitebox::getCodingBuf(index1), "alddd", 5) == 0);

    size_t size2 = SplitIndex1CompWhitebox::decodeToBuf(index1, toDecode.c_str(), toDecode.size(), 2);
    REQUIRE(size2 == 0); // Exceeded expected max word size.
}

} // namespace split_index
