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

TEST_CASE("is storing prefix and suffix in buffers correct for even size", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);
    index.construct();

    SplitIndex1Whitebox::storePrefixSuffixInBuffers(index, "pies");

    REQUIRE(SplitIndex1Whitebox::getPrefixSize(index) == 2);
    REQUIRE(SplitIndex1Whitebox::getSuffixSize(index) == 2);

    REQUIRE(strncmp(SplitIndex1Whitebox::getPrefixBuf(index), "pi", 2) == 0);
    REQUIRE(strncmp(SplitIndex1Whitebox::getSuffixBuf(index), "es", 2) == 0);
}

TEST_CASE("is storing prefix and suffix in buffers correct for odd size", "[split_index_1]")
{
    SplitIndex1 index({ "index" }, hashType, 1.0f);
    index.construct();

    SplitIndex1Whitebox::storePrefixSuffixInBuffers(index, "piesa");

    REQUIRE(SplitIndex1Whitebox::getPrefixSize(index) == 2);
    REQUIRE(SplitIndex1Whitebox::getSuffixSize(index) == 3);

    REQUIRE(strncmp(SplitIndex1Whitebox::getPrefixBuf(index), "pi", 2) == 0);
    REQUIRE(strncmp(SplitIndex1Whitebox::getSuffixBuf(index), "esa", 3) == 0);
}

} // namespace split_index
