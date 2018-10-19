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

TEST_CASE("is empty index correctly initialized", "[split_index_1]")
{
    SplitIndex1 index({ "ala" }, hashType, 1.0f);
    REQUIRE(index.calcWordsSizeB() == 3);
}

} // namespace split_index
