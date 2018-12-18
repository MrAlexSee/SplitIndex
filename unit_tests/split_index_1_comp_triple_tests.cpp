#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "split_index_1_comp_whitebox.hpp"

#include "../src/index/split_index_1_comp_triple.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;
constexpr int maxNIter = 10;

}

TEST_CASE("does split index 1 comp triple throw for empty words", "[split_index_1_comp_triple]")
{
    REQUIRE_THROWS(SplitIndex1CompTriple({ }, hashType, 1.0f));
}

TEST_CASE("is calculating 2- and 3-grams ordered by frequency correct", "[split_index_1_comp_triple]")
{
    const unordered_set<string> wordSet{ "ala", "dla", "kotala", "jarek", "lubi", "psy" };    
    SplitIndex1CompTriple index1(wordSet, hashType, 1.0f);
    
    vector<string> qgrams2 = SplitIndex1CompWhitebox::calcQGramsOrderedByFrequency(index1, 100, 2);
    const vector<string> expected2 { "al", "la", "dl", "ko", "ot", "ta", "ja", "ar", "re", "ek", "lu",
        "ub", "bi", "ps", "sy" };

    REQUIRE(qgrams2.size() == expected2.size());

    for (const string &qgram : expected2)
    {
        REQUIRE(find(qgrams2.begin(), qgrams2.end(), qgram) != qgrams2.end());
    }

    // "la" occurs twice so it should be the first one.
    REQUIRE(qgrams2.front() == "la");

    vector<string> qgrams3 = SplitIndex1CompWhitebox::calcQGramsOrderedByFrequency(index1, 100, 3);
    const vector<string> expected3 { "ala", "dla", "kot", "ota", "tal", "jar", "are", "rek", "lub", "ubi", "psy" };

    // "ala" occurs twice so it should be the first one.
    REQUIRE(qgrams3.front() == "ala");
}

TEST_CASE("is filling 2,3,4-gram maps correct", "[split_index_1_comp_triple]")
{
    const unordered_set<string> wordSet{ "ala", "dla", "kotala", "jarek", "lubi", "psy" };    

    SplitIndex1CompTriple index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // This assumes that the compile-time number of 2-,3-,4-grams is >= than the number of q-grams extracted from words.
    const vector<string> expected { "al", "la", "dl", "ko", "ot", "ta", "ja", "ar", "re", "ek", "lu",
        "ub", "bi", "ps", "sy",
        "ala", "dla", "kot", "ota", "tal", "jar", "are", "rek", "lub", "ubi", "psy",
        "kota", "otal", "tala", "jare", "arek", "lubi" };

    const map<string, char> qgramToChar = SplitIndex1CompWhitebox::getQGramToCharMap(index1);
    REQUIRE(qgramToChar.size() == expected.size());

    for (const string &qgram : expected)
    {
        REQUIRE(qgramToChar.find(qgram) != qgramToChar.end());
    }

    for (const string &qgramOut : { "aa", "cc", "dota" })
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

TEST_CASE("is triple encoding to buffer correct", "[split_index_1_comp_triple]")
{
    const unordered_set<string> wordSet{ "ala", "ma", "kota" };

    SplitIndex1CompTriple index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // 2-grams used for encoding: al, la, ma, ko, ot, ta.
    // 3-grams used for encoding: ala, kot, ota.
    // 4-grams used for encoding: kota.

    size_t size1 = SplitIndex1CompWhitebox::encodeToBuf(index1, "alddd", 5);
    char encoded1 = SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("al");

    const string expected1 = string(1, encoded1) + "ddd";

    REQUIRE(size1 == expected1.size());
    REQUIRE(memcmp(SplitIndex1CompWhitebox::getCodingBuf(index1), expected1.c_str(), expected1.size()) == 0);

    size_t size2 = SplitIndex1CompWhitebox::encodeToBuf(index1, "kotamakoala", 11);
    
    char encodedChars2[] { SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("kota"),
        SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("ma"),
        SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("ko"),
        SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("ala") };

    const string expected2 = string(1, encodedChars2[0]) + string(1, encodedChars2[1]) + 
        string(1, encodedChars2[2]) + string(1, encodedChars2[3]);

    REQUIRE(size2 == expected2.size());
    REQUIRE(memcmp(SplitIndex1CompWhitebox::getCodingBuf(index1), expected2.c_str(), expected2.size()) == 0);
}

TEST_CASE("is triple decoding to buffer correct", "[split_index_1_comp_triple]")
{
    const unordered_set<string> wordSet{ "ala", "ma", "kota" };

    SplitIndex1CompTriple index1(wordSet, hashType, 1.0f);
    SplitIndex1CompWhitebox::calcQgramsAndFillMaps(index1);

    // 2-grams used for encoding: al, la, ma, ko, ot, ta.
    // 3-grams used for encoding: ala, kot, ota.
    // 4-grams used for encoding: kota.

    char encodedChars[] { SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("ma"),
        SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("ala"),
        SplitIndex1CompWhitebox::getQGramToCharMap(index1).at("kota") };

    const string toDecode = string(1, encodedChars[0]) + string(1, encodedChars[1]) + 
        string(1, encodedChars[2]) + "ddd";

    size_t size1 = SplitIndex1CompWhitebox::decodeToBuf(index1, toDecode.c_str(), toDecode.size(), 12);

    REQUIRE(size1 == 12);
    REQUIRE(memcmp(SplitIndex1CompWhitebox::getCodingBuf(index1), "maalakotaddd", 12) == 0);

    size_t size2 = SplitIndex1CompWhitebox::decodeToBuf(index1, toDecode.c_str(), toDecode.size(), 2);
    REQUIRE(size2 == 0); // Exceeded expected max word size.
}

} // namespace split_index
