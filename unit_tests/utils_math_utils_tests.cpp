#include "catch.hpp"

#include "../src/utils/math_utils.hpp"

using namespace std;

namespace split_index
{

namespace
{

constexpr int maxNRepeats = 5;

}

TEST_CASE("is Cartesian product calculation for empty correct", "[utils_math]")
{
    vector<int> empty;

    for (int i = 2; i < maxNRepeats; ++i)
    {
        auto prod = utils::MathUtils::calcCartesianProduct(empty, i);
        REQUIRE(prod.empty());
    }
}

TEST_CASE("is Cartesian product calculation for 2 repeats correct", "[utils_math]")
{
    vector<int> vec { 1, 2, 3, 4 };
    auto prod = utils::MathUtils::calcCartesianProduct(vec, 2);

    // TODO
}

} // namespace split_index
