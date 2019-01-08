#include "catch.hpp"
#include "repeat.hpp"

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

TEST_CASE("is searching empty patterns correct for k > 1", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "alama", "kota", "jarek", "lubi" };
    SplitIndex *indexes[] = {
        new SplitIndexK<2>(wordSet, hashType, 1.0f),
        new SplitIndexK<3>(wordSet, hashType, 1.0f) };

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

TEST_CASE("is searching words exact correct for k > 1", "[split_index_k_searching]")
{
    const vector<string> words { "jarek", "lubi", "koty" };
    const vector<string> patternsOut { "this", "dict" };

    SplitIndex *indexes[] = {
        new SplitIndexK<2>({ words.begin(), words.end() }, hashType, 1.0f),
        new SplitIndexK<3>({ words.begin(), words.end() }, hashType, 1.0f) };

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

TEST_CASE("is searching words exact one-by-one correct for k > 1", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "jarek", "lubi", "koty" };
    const vector<string> patternsOut { "this", "dict" };

    SplitIndex *indexes[] = {
        new SplitIndexK<2>(wordSet, hashType, 1.0f),
        new SplitIndexK<3>(wordSet, hashType, 1.0f) };

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

TEST_CASE("is searching for k > 1 for 1 error correct", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "alama", "lubi", "kota", "jarek", "piesa" };

    SplitIndex *indexes[] = {
        new SplitIndexK<2>(wordSet, hashType, 1.0f),
        new SplitIndexK<3>(wordSet, hashType, 1.0f) };

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

TEST_CASE("is searching for k = 2 for 2 errors correct", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "ala", "kota", "jarek", "psa", "bardzo", "lubie", "owoce" };

    SplitIndexK<2> index(wordSet, hashType, 1.0f);
    index.construct();

    for (const string &word : wordSet)
    {
        for (size_t i = 0; i < word.size(); ++i)
        {
            string curWord = word;
            
            curWord[i] = 'N';
            if (i != word.size() - 1)
            {
                curWord[i + 1] = 'N';
            }
            else
            {
                curWord[0] = 'N';
            }

            for (int nIter = 1; nIter <= maxNIter; ++nIter)
            {
                const set<string> result = index.search({ curWord }, nIter);
                REQUIRE(result.size() >= 1);

                for (const string &word : result)
                {
                    REQUIRE(wordSet.find(word) != wordSet.end());
                }
            }
        }
    }
}

TEST_CASE("is searching words for k = 2 for various number of mismatches correct", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "ala", "kota", "jarek", "psa", "bardzo", "lubie", "owoce" };

    SplitIndexK<2> indexk2(wordSet, hashType, 1.0f);
    indexk2.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(indexk2.search({ "ada", "pzz" }, nIter) == set<string>{ "ala", "psa" });
        REQUIRE(indexk2.search({ "ccc", "zzz" }, nIter).empty());

        REQUIRE(indexk2.search({ "jacek" }, nIter) == set<string>{ "jarek" });
        REQUIRE(indexk2.search({ "gacek" }, nIter) == set<string>{ "jarek" });
        REQUIRE(indexk2.search({ "jacek", "jadek", "japek", "gacek" }, nIter) == set<string>{ "jarek" });

        REQUIRE(indexk2.search({ "pccek", "jaxyz" }, nIter).empty());

        REQUIRE(indexk2.search({ "bardzo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(indexk2.search({ "barczo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(indexk2.search({ "darczo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(indexk2.search({ "barczo", "dardzo", "aardzo" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(indexk2.search({ "darcza",  }, nIter).empty());
    }
}

TEST_CASE("is searching words for k = 3 for various number of mismatches correct", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet { "kota", "jarek", "bardzo", "lubie", "owoce" };

    SplitIndexK<3> indexk3(wordSet, hashType, 1.0f);
    indexk3.construct();

    for (int nIter = 1; nIter <= maxNIter; ++nIter)
    {
        REQUIRE(indexk3.search({ "kott" }, nIter) == set<string>{ "kota" });
        REQUIRE(indexk3.search({ "ccca" }, nIter) == set<string>{ "kota" });

        REQUIRE(indexk3.search({ "owoce" }, nIter) == set<string>{ "owoce" });
        REQUIRE(indexk3.search({ "owode" }, nIter) == set<string>{ "owoce" });
        REQUIRE(indexk3.search({ "owddd" }, nIter) == set<string>{ "owoce" });
        REQUIRE(indexk3.search({ "odddd" }, nIter).empty());

        REQUIRE(indexk3.search({ "jaddd" }, nIter) == set<string>{ "jarek" });
        REQUIRE(indexk3.search({ "barccc" }, nIter) == set<string>{ "bardzo" });
        REQUIRE(indexk3.search({ "aaaaa" }, nIter).empty());
    }
}

} // namespace split_index
