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
constexpr int maxK = 3;

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

} // namespace split_index
