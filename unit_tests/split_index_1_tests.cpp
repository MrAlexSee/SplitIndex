#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "split_index_1_whitebox.hpp"

#include "../src/index/split_index_1.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;

}

TEST_CASE("does index throw for empty words", "[split_index_1]")
{
    REQUIRE_THROWS(SplitIndex1({ }, hashType, 1.0f));
}

TEST_CASE("is word size correctly initialized", "[split_index_1]")
{
    SplitIndex1 index1({ "ala" }, hashType, 1.0f);
    REQUIRE(index1.calcWordsSizeB() == 3);

    SplitIndex1 index2({ "ala", "ma", "kota", "jarek", "da", "psa" }, hashType, 1.0f);
    REQUIRE(index2.calcWordsSizeB() == 19);
}

TEST_CASE("is searching words exact correct", "[split_index_1]")
{
    vector<string> words { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex1 index1(unordered_set<string>(words.begin(), words.end()), hashType, 1.0f);
    index1.construct();

    string results;

    REQUIRE(index1.search(words, results) == words.size());
    REQUIRE(index1.search(patternsOut, results) == 0);
}

TEST_CASE("is searching words exact one-by-one correct", "[split_index_1]")
{
    vector<string> words { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex1 index1(unordered_set<string>(words.begin(), words.end()), hashType, 1.0f);
    index1.construct();

    string results;

    for (const string &word : words)
    {
        REQUIRE(index1.search({ word }, results) == 1);
    }

    for (const string &patternOut : patternsOut)
    {
        REQUIRE(index1.search({ patternOut }, results) == 0);
    }
}

TEST_CASE("is entry size calculation correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);
    REQUIRE(SplitIndex1Whitebox::calcEntrySizeB(index, entry) == 7);

    SplitIndex1Whitebox::addToEntry(index, &entry, "ada", 3, true);
    REQUIRE(SplitIndex1Whitebox::calcEntrySizeB(index, entry) == 11);

    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);
    REQUIRE(SplitIndex1Whitebox::calcEntrySizeB(index, entry) == 17);

    SplitIndex1Whitebox::addToEntry(index, &entry, "pies", 4, false);
    REQUIRE(SplitIndex1Whitebox::calcEntrySizeB(index, entry) == 22);
}

TEST_CASE("is entry word count calculation correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);
    REQUIRE(SplitIndex1Whitebox::calcEntryNWords(index, entry) == 1);

    SplitIndex1Whitebox::addToEntry(index, &entry, "ada", 3, true);
    REQUIRE(SplitIndex1Whitebox::calcEntryNWords(index, entry) == 2);

    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);
    REQUIRE(SplitIndex1Whitebox::calcEntryNWords(index, entry) == 3);

    SplitIndex1Whitebox::addToEntry(index, &entry, "pies", 4, false);
    REQUIRE(SplitIndex1Whitebox::calcEntryNWords(index, entry) == 4);
}

TEST_CASE("is storing prefix and suffix in buffers correct for even size", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);
    index.construct();

    SplitIndex1Whitebox::storePrefixSuffixInBuffers(index, "pies");

    REQUIRE(SplitIndex1Whitebox::getPrefixSize(index) == 2);
    REQUIRE(SplitIndex1Whitebox::getSuffixSize(index) == 2);

    REQUIRE(memcmp(SplitIndex1Whitebox::getPrefixBuf(index), "pi", 2) == 0);
    REQUIRE(memcmp(SplitIndex1Whitebox::getSuffixBuf(index), "es", 2) == 0);
}

TEST_CASE("is storing prefix and suffix in buffers correct for odd size", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);
    index.construct();

    SplitIndex1Whitebox::storePrefixSuffixInBuffers(index, "piesa");

    REQUIRE(SplitIndex1Whitebox::getPrefixSize(index) == 2);
    REQUIRE(SplitIndex1Whitebox::getSuffixSize(index) == 3);

    REQUIRE(memcmp(SplitIndex1Whitebox::getPrefixBuf(index), "pi", 2) == 0);
    REQUIRE(memcmp(SplitIndex1Whitebox::getSuffixBuf(index), "esa", 3) == 0);
}

TEST_CASE("is creating suffix entry correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);
    REQUIRE(memcmp(entry, "\0\0\3ala\0", 7) == 0);
}

TEST_CASE("is creating prefix entry correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, false);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 1u);
    REQUIRE(memcmp(entry + 2, "\3ala\0", 5) == 0);
}

TEST_CASE("is adding to entry only suffixes correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "ba", 2, true);

    REQUIRE(memcmp(entry, "\0\0\3ala\5index\2ba\0", 16) == 0);
}

TEST_CASE("is adding to entry only prefixes correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "ba", 2, false);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 1u);
    REQUIRE(memcmp(entry + 2, "\3ala\5index\2ba\0", 14) == 0);
}

TEST_CASE("is adding to entry prefixes and suffixes correct 1", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, false);

    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "pies", 4, true);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 3u);
    REQUIRE(memcmp(entry + 2, "\5index\4pies\3ala\0", 16) == 0);
}

TEST_CASE("is adding to entry prefixes and suffixes correct 2", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);
 
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "pies", 4, false);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 2u);
    REQUIRE(memcmp(entry + 2, "\3ala\5index\4pies\0", 16) == 0);
}

TEST_CASE("is advancing by word count in entry with prefixes correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, false);

    SplitIndex1Whitebox::addToEntry(index, &entry, "ada", 3, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "dla", 3, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);

    char *advanced1 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 1);
    REQUIRE(advanced1 == entry + 6);

    char *advanced2 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 2);
    REQUIRE(advanced2 == entry + 10);

    char *advanced3 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 3);
    REQUIRE(advanced3 == entry + 14);
}

TEST_CASE("is advancing by word count in entry with suffixes correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);

    SplitIndex1Whitebox::addToEntry(index, &entry, "ada", 3, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "dla", 3, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, true);

    char *advanced1 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 1);
    REQUIRE(advanced1 == entry + 6);

    char *advanced2 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 2);
    REQUIRE(advanced2 == entry + 10);

    char *advanced3 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 3);
    REQUIRE(advanced3 == entry + 14);
}

TEST_CASE("is advancing by word count in entry with prefixes and suffixes correct", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);

    char *entry = SplitIndex1Whitebox::createEntry(index, "ala", 3, true);

    SplitIndex1Whitebox::addToEntry(index, &entry, "ada", 3, false);
    SplitIndex1Whitebox::addToEntry(index, &entry, "dla", 3, true);
    SplitIndex1Whitebox::addToEntry(index, &entry, "index", 5, false);

    char *advanced1 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 1);
    REQUIRE(advanced1 == entry + 6);

    char *advanced2 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 2);
    REQUIRE(advanced2 == entry + 10);

    char *advanced3 = SplitIndex1Whitebox::advanceInEntryByWordCount(index, entry + 2, 3);
    REQUIRE(advanced3 == entry + 14);
}

} // namespace split_index
