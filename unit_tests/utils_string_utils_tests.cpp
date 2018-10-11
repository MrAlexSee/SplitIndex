#include <string>
#include <vector>

#include "catch.hpp"
#include "repeat.hpp"

#include "../src/utils/string_utils.hpp"

using namespace std;

namespace split_index
{

namespace
{

constexpr int nRandIter = 100;

}

TEST_CASE("is word filtering by minimum length for empty vector correct", "[utils]")
{
    vector<string> empty;

    utils::StringUtils::filterWordsByMinLength(empty, 1);
    REQUIRE(empty.empty());
}

TEST_CASE("is word filtering by minimum length correct", "[utils]")
{
    vector<string> vec { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };

    utils::StringUtils::filterWordsByMinLength(vec, 2);
    REQUIRE(vec.size() == 6);

    utils::StringUtils::filterWordsByMinLength(vec, 4);
    REQUIRE(vec.size() == 2);
}

TEST_CASE("is empty vector to string correct", "[utils]")
{
    string str1 = utils::StringUtils::vecToStr(vector<int>(), " ");
    REQUIRE(str1 == "[]");
}

TEST_CASE("is vector to string correct", "[utils]")
{
    vector<int> single { 7 };

    string str1 = utils::StringUtils::vecToStr(single, ", ");
    REQUIRE(str1 == "[7]");

    vector<int> vec { 1, 3, 4, 7 };

    string str2 = utils::StringUtils::vecToStr(vec, ", ");
    REQUIRE(str2 == "[1, 3, 4, 7]");
}

} // namespace split_index
