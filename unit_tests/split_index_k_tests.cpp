#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "split_index_k_whitebox.hpp"

#include "../src/index/split_index_k.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;
constexpr size_t maxK = 3;

}

TEST_CASE("does split index k throw for bad k", "[split_index_k]")
{
    REQUIRE_THROWS(SplitIndexK<0>({ }, hashType, 1.0f));
    REQUIRE_THROWS(SplitIndexK<100>({ }, hashType, 1.0f));
}

TEST_CASE("does split index k throw for empty words", "[split_index_k]")
{
    for_<maxK + 1>([&] (auto k)
    {
        if (k.value > 0)
        {
            REQUIRE_THROWS(SplitIndexK<k.value>({ }, hashType, 1.0f));
        }
    });
}

TEST_CASE("is k entry size calculation correct", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    char *entry = SplitIndexKWhitebox::createEntry<1>("ala", 3, 0);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 8);

    // 4 words in total -- no additional part bytes added.

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "ada", 3, 0);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 12);

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "index", 5, false);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 18);

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "pies", 4, false);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 23);

    // 5 words in total -- one additional part byte added.

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "smok", 4, false);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 29);
}

TEST_CASE("is part size calculation correct", "[split_index_k]")
{    
    REQUIRE(SplitIndexKWhitebox::getPartSize<1>(4) == 2);
    REQUIRE(SplitIndexKWhitebox::getPartSize<2>(4) == 1);

    REQUIRE(SplitIndexKWhitebox::getPartSize<1>(10) == 5);
    REQUIRE(SplitIndexKWhitebox::getPartSize<2>(10) == 3);
}

} // namespace split_index
