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
constexpr int maxNIter = 10;

}

TEST_CASE("does split index 1 throw for empty words", "[split_index_1]")
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

TEST_CASE("is searching empty patterns correct", "[split_index_1]")
{
    unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "lubi", "psy" };

    SplitIndex1 index1(wordSet, hashType, 1.0f);
    index1.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(index1.search({ }, nIter).empty());
    }
}

TEST_CASE("is searching words exact correct", "[split_index_1]")
{
    vector<string> words { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex1 index1({ words.begin(), words.end() }, hashType, 1.0f);
    index1.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(index1.search(words, nIter) == set<string>(words.begin(), words.end()));
        REQUIRE(index1.search(patternsOut, nIter).empty());
    }
}

TEST_CASE("is searching words exact one-by-one correct", "[split_index_1]")
{
    unordered_set<string> wordSet{ "ala", "ma", "kota", "jarek", "lubi", "psy" };
    vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex1 index1(wordSet, hashType, 1.0f);
    index1.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        for (const string &word : wordSet)
        {
            REQUIRE(index1.search({ word }, nIter) == set<string>{ word });
        }

        for (const string &patternOut : patternsOut)
        {
            REQUIRE(index1.search({ patternOut }, nIter).empty());
        }
    }
}

TEST_CASE("is searching words for k = 1 for Hamming correct", "[split_index_1]")
{
    unordered_set<string> wordSet{ "ala", "ma", "kota", "jarek", "psa" };
    vector<string> patternsIn;

    SplitIndex1 index1(wordSet, hashType, 1.0f);
    index1.construct();

    for (const string &word : wordSet)
    {
        for (size_t i = 0; i < word.size(); ++i)
        {
            string curWord = word;
            curWord[i] = 'N';

            patternsIn.push_back(move(curWord));
        }
    }

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(index1.search(patternsIn, nIter) == set<string>(wordSet.begin(), wordSet.end()));
    }
}

TEST_CASE("is searching words for k = 1 one-by-one for Hamming correct", "[split_index_1]")
{
    unordered_set<string> wordSet{ "ala", "ma", "kota", "jarek", "psa" };

    SplitIndex1 index1(wordSet, hashType, 1.0f);
    index1.construct();

    for (const string &word : wordSet)
    {
        for (size_t i = 0; i < word.size(); ++i)
        {
            string curWord = word;
            curWord[i] = 'N';

            for (int nIter = 1; nIter <= maxNIter; ++nIter)
            {
                set<string> result = index1.search({ curWord }, nIter);
                REQUIRE(result.size() == 1);

                for (const string &word : result)
                {
                    REQUIRE(wordSet.find(word) != wordSet.end());
                }
            }
        }
    }
}

TEST_CASE("is searching words for various k for Hamming correct", "[split_index_1]")
{
    unordered_set<string> wordSet{ "ala", "ma", "kota", "jarek", "psa", "bardzo", "lubie", "owoce" };

    SplitIndex1 index1(wordSet, hashType, 1.0f);
    index1.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(index1.search({ "da", "la", "fa" }, nIter) == set<string>{ "ma" });
        REQUIRE(index1.search({ "bb", "cc" }, nIter).empty());

        REQUIRE(index1.search({ "osa", "ada" }, nIter) == set<string>{ "ala", "psa" });
        REQUIRE(index1.search({ "bbb", "ccc", "ddd" }, nIter).empty());

        REQUIRE(index1.search({ "darek", "japek", "jacek", "barek" }, nIter) == set<string>{ "jarek" });
        REQUIRE(index1.search({ "czarek", "bapek", "kapek" }, nIter).empty());

        REQUIRE(index1.search({ "bardzo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(index1.search({ "barzzo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(index1.search({ "kardzo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(index1.search({ "karzzo", "bordza" }, nIter).empty());
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
