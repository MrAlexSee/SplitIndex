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

TEST_CASE("is searching for k = 2 correct", "[split_index_k_searching]")
{
    const unordered_set<string> wordSet{ "ala", "ma", "kota", "jarek", "psa", "bardzo", "lubie", "owoce" };

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
                set<string> result = index.search({ curWord }, nIter);
                REQUIRE(result.size() == 1);

                for (const string &word : result)
                {
                    REQUIRE(wordSet.find(word) != wordSet.end());
                }
            }
        }
    }
}

} // namespace split_index
