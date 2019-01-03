#include "catch.hpp"
#include "repeat.hpp"

#include "../src/index/split_index_1.hpp"
#include "../src/index/split_index_1_comp.hpp"
#include "../src/index/split_index_1_comp_triple.hpp"
#include "../src/index/split_index_k.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;
constexpr int maxNIter = 10;

}

TEST_CASE("is searching empty patterns correct", "[split_index_1_searching]")
{
    const unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    SplitIndex *indexes[] = { 
        new SplitIndex1(wordSet, hashType, 1.0f), 
        new SplitIndex1Comp(wordSet, hashType, 1.0f),
        new SplitIndex1CompTriple(wordSet, hashType, 1.0f),
        new SplitIndexK<1>(wordSet, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

        for (int nIter = 1; nIter <= maxNIter; ++nIter)
        {
            REQUIRE(indexes[iIndex]->search({ }, nIter).empty());
        }

        delete indexes[iIndex];
    }
}

TEST_CASE("is searching words exact correct", "[split_index_1_searching]")
{
    const vector<string> words { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    const vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex *indexes[] = { 
        new SplitIndex1({ words.begin(), words.end() }, hashType, 1.0f), 
        new SplitIndex1Comp({ words.begin(), words.end() }, hashType, 1.0f),
        new SplitIndex1CompTriple({ words.begin(), words.end() }, hashType, 1.0f),
        new SplitIndexK<1>({ words.begin(), words.end() }, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

        for (int nIter = 1; nIter <= maxNIter; ++nIter)
        {
            REQUIRE(indexes[iIndex]->search(words, nIter) == set<string>(words.begin(), words.end()));
            REQUIRE(indexes[iIndex]->search(patternsOut, nIter).empty());
        }

        delete indexes[iIndex];
    }
}

TEST_CASE("is searching words exact one-by-one correct", "[split_index_1_searching]")
{
    const unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "lubi", "psy" };
    const vector<string> patternsOut { "not", "in", "this", "dict" };

    SplitIndex *indexes[] = { 
        new SplitIndex1(wordSet, hashType, 1.0f), 
        new SplitIndex1Comp(wordSet, hashType, 1.0f),
        new SplitIndex1CompTriple(wordSet, hashType, 1.0f),
        new SplitIndexK<1>(wordSet, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

        for (int nIter = 1; nIter <= maxNIter; ++nIter)
        {
            for (const string &word : wordSet)
            {
                REQUIRE(indexes[iIndex]->search({ word }, nIter) == set<string>{ word });
            }

            for (const string &patternOut : patternsOut)
            {
                REQUIRE(indexes[iIndex]->search({ patternOut }, nIter).empty());
            }
        }

        delete indexes[iIndex];
    }
}

TEST_CASE("is searching words for k = 1 for 1 error correct", "[split_index_1_searching]")
{
    const unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "psa" };
    vector<string> patternsIn;

    SplitIndex *indexes[] = { 
        new SplitIndex1(wordSet, hashType, 1.0f), 
        new SplitIndex1Comp(wordSet, hashType, 1.0f),
        new SplitIndex1CompTriple(wordSet, hashType, 1.0f),
        new SplitIndexK<1>(wordSet, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

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
            REQUIRE(indexes[iIndex]->search(patternsIn, nIter) == set<string>(wordSet.begin(), wordSet.end()));
        }

        delete indexes[iIndex];
    }
}

TEST_CASE("is searching words for k = 1 for 1 error one-by-one correct", "[split_index_1_searching]")
{
    const unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "psa" };

    SplitIndex *indexes[] = { 
        new SplitIndex1(wordSet, hashType, 1.0f), 
        new SplitIndex1Comp(wordSet, hashType, 1.0f),
        new SplitIndex1CompTriple(wordSet, hashType, 1.0f),
        new SplitIndexK<1>(wordSet, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

        for (const string &word : wordSet)
        {
            for (size_t i = 0; i < word.size(); ++i)
            {
                string curWord = word;
                curWord[i] = 'N';

                for (int nIter = 1; nIter <= maxNIter; ++nIter)
                {
                    const set<string> result = indexes[iIndex]->search({ curWord }, nIter);

                    REQUIRE(result.size() == 1);
                    REQUIRE(wordSet.find(*result.begin()) != wordSet.end());
                }
            }
        }

        delete indexes[iIndex];
    }
}

TEST_CASE("is searching words for k = 1 for various number of mismatches correct", "[split_index_1_searching]")
{
    const unordered_set<string> wordSet { "ala", "ma", "kota", "jarek", "psa", "bardzo", "lubie", "owoce" };

    SplitIndex *indexes[] = { 
        new SplitIndex1(wordSet, hashType, 1.0f), 
        new SplitIndex1Comp(wordSet, hashType, 1.0f),
        new SplitIndex1CompTriple(wordSet, hashType, 1.0f),
        new SplitIndexK<1>(wordSet, hashType, 1.0f) };

    const int nIndexes = sizeof(indexes) / sizeof(indexes[0]);

    for (int iIndex = 0; iIndex < nIndexes; ++iIndex)
    {
        indexes[iIndex]->construct();

        for (int nIter = 1; nIter <= maxNIter; ++nIter)
        {
            REQUIRE(indexes[iIndex]->search({ "da", "la", "fa" }, nIter) == set<string>{ "ma" });
            REQUIRE(indexes[iIndex]->search({ "bb", "cc" }, nIter).empty());

            REQUIRE(indexes[iIndex]->search({ "osa", "ada" }, nIter) == set<string>{ "ala", "psa" });
            REQUIRE(indexes[iIndex]->search({ "bbb", "ccc", "ddd" }, nIter).empty());

            REQUIRE(indexes[iIndex]->search({ "darek" }, nIter) == set<string>{ "jarek" });
            REQUIRE(indexes[iIndex]->search({ "barek" }, nIter) == set<string>{ "jarek" });
            REQUIRE(indexes[iIndex]->search({ "darek", "japek", "jacek", "barek" }, nIter) == set<string>{ "jarek" });
            REQUIRE(indexes[iIndex]->search({ "czarek", "bapek", "kapek" }, nIter).empty());

            REQUIRE(indexes[iIndex]->search({ "bardzo" }, nIter) == set<string>{ "bardzo" });
            REQUIRE(indexes[iIndex]->search({ "barzzo" }, nIter) == set<string>{ "bardzo" });
            REQUIRE(indexes[iIndex]->search({ "kardzo" }, nIter) == set<string>{ "bardzo" });
            REQUIRE(indexes[iIndex]->search({ "karzzo", "bordza" }, nIter).empty());
        }

        delete indexes[iIndex];
    }
}

} // namespace split_index
