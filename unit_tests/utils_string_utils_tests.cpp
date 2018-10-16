#include <string>
#include <vector>

#include "catch.hpp"

#include "../src/utils/string_utils.hpp"

using namespace std;

namespace split_index
{

TEST_CASE("is getting elapsed info correct", "[utils_string_utils]")
{
    string info1 = utils::StringUtils::getElapsedInfo(10000, 1, 5);

    REQUIRE(info1.find("10") != string::npos); // per iter ms
    REQUIRE(info1.find("1") != string::npos); // #iter
    REQUIRE(info1.find("2000") != string::npos); // per query us

    string info2 = utils::StringUtils::getElapsedInfo(10000, 5, 20);

    REQUIRE(info2.find("2") != string::npos); // per iter ms
    REQUIRE(info2.find("5") != string::npos); // #iter
    REQUIRE(info2.find("100") != string::npos); // per query us
}

TEST_CASE("is word filtering by minimum length for empty vector correct", "[utils_string_utils]")
{
    vector<string> empty;

    utils::StringUtils::filterWordsByMinLength(empty, 1);
    REQUIRE(empty.empty());
}

TEST_CASE("is word filtering by minimum length correct", "[utils_string_utils]")
{
    vector<string> vec { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };

    utils::StringUtils::filterWordsByMinLength(vec, 2);
    REQUIRE(vec.size() == 6);

    utils::StringUtils::filterWordsByMinLength(vec, 4);
    REQUIRE(vec.size() == 2);
}

TEST_CASE("is empty vector to string correct", "[utils_string_utils]")
{
    string str1 = utils::StringUtils::vecToStr(vector<int>(), " ");
    REQUIRE(str1 == "[]");
}

TEST_CASE("is vector to string correct", "[utils_string_utils]")
{
    vector<int> single { 7 };

    string str1 = utils::StringUtils::vecToStr(single, ", ");
    REQUIRE(str1 == "[7]");

    vector<int> vec { 1, 3, 4, 7 };

    string str2 = utils::StringUtils::vecToStr(vec, ", ");
    REQUIRE(str2 == "[1, 3, 4, 7]");

    string str3 = utils::StringUtils::vecToStr(vec, ";");
    REQUIRE(str3 == "[1;3;4;7]");
}

} // namespace split_index
