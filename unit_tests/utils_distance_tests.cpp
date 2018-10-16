#include "catch.hpp"

#include "../src/utils/distance.hpp"

using namespace std;

namespace split_index
{

TEST_CASE("is exact match calculation correct", "[utils]")
{
    REQUIRE(utils::Distance::isExactMatch("", "", 0));
}

} // namespace split_index
