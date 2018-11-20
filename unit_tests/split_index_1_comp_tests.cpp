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
constexpr int maxNIter = 10;

}

TEST_CASE("does split index 1 comp throw for empty words", "[split_index_1_comp]")
{
    REQUIRE_THROWS(SplitIndex1Comp({ }, hashType, 1.0f));
}

TEST_CASE("is calculating qgrams ordered by frequency correct", "[split_index_1_comp]")
{
    // TODO
}

} // namespace split_index
