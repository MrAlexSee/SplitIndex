#include <array>
#include <random>
#include <set>

#include "catch.hpp"
#include "repeat.hpp"

#include "../src/utils/distance.hpp"

using namespace std;

namespace split_index
{

namespace
{

constexpr int maxK = 5;
constexpr int nHammingRepeats = 10;

}

TEST_CASE("is Hamming at most k for empty calculation correct", "[utils_distance]")
{
    string empty = "";

    for_<maxK + 1>([&] (auto k)
    {
        REQUIRE(utils::Distance::isHammingAtMostK<k.value>(empty.c_str(), empty.c_str(), 0) == true);    
    });
}

TEST_CASE("is Hamming at most k calculation for self correct", "[utils_distance]")
{
    string str1 = "ala ma kota";

    for_<maxK + 1>([&] (auto k)
    {
        REQUIRE(utils::Distance::isHammingAtMostK<k.value>(str1.c_str(), str1.c_str(), str1.size()) == true);    
    });
}

TEST_CASE("is Hamming at most k=1 calculation correct", "[utils_distance]")
{
    const string str = "ala ma kota";
    
    for (size_t i = 0; i < str.size(); ++i)
    {
        string cur = str;
        cur[i] = 'N';

        REQUIRE(utils::Distance::isHammingAtMostK<0>(cur.c_str(), str.c_str(), cur.size()) == false);
        REQUIRE(utils::Distance::isHammingAtMostK<0>(str.c_str(), cur.c_str(), cur.size()) == false);

        REQUIRE(utils::Distance::isHammingAtMostK<1>(cur.c_str(), str.c_str(), cur.size()) == true);
        REQUIRE(utils::Distance::isHammingAtMostK<1>(str.c_str(), cur.c_str(), cur.size()) == true);
    }
}

TEST_CASE("is Hamming at most k=1 calculation for selected words correct", "[distance]")
{
    string str1 = "aaaa", str2 = "aaab";
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str1.c_str(), str2.c_str(), str1.size()) == true);
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str2.c_str(), str1.c_str(), str1.size()) == true);

    str1 = "aaaa", str2 = "baab";
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str1.c_str(), str2.c_str(), str1.size()) == false);
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str2.c_str(), str1.c_str(), str1.size()) == false);

    str1 = "abcdef", str2 = "abccef";
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str1.c_str(), str2.c_str(), str1.size()) == true);
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str2.c_str(), str1.c_str(), str1.size()) == true);

    str1 = "abcdef", str2 = "abcccf";
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str1.c_str(), str2.c_str(), str1.size()) == false);
    REQUIRE(utils::Distance::isHammingAtMostK<1>(str2.c_str(), str1.c_str(), str1.size()) == false);
}

TEST_CASE("is Hamming at most k=2 calculation for selected words correct", "[utils_distance]")
{
    string str1 = "aaaa", str2 = "aabb";
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str1.c_str(), str2.c_str(), str1.size()) == true);
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str2.c_str(), str1.c_str(), str1.size()) == true);

    str1 = "aaaa", str2 = "abbb";
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str1.c_str(), str2.c_str(), str1.size()) == false);
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str2.c_str(), str1.c_str(), str1.size()) == false);

    str1 = "abcdef", str2 = "abcccf";
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str1.c_str(), str2.c_str(), str1.size()) == true);
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str2.c_str(), str1.c_str(), str1.size()) == true);

    str1 = "abcdef", str2 = "abcccc";
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str1.c_str(), str2.c_str(), str1.size()) == false);
    REQUIRE(utils::Distance::isHammingAtMostK<2>(str2.c_str(), str1.c_str(), str1.size()) == false);
}

TEST_CASE("is Hamming at most k=0,1,2,3,4 randomized calculation correct", "[utils_distance]")
{
    auto randNumbersFromRange = [](int start, int end, size_t count) {
        set<int> ret;

        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<int> dist(start, end);

        while (ret.size() < count)
        {
            ret.insert(dist(mt));
        }

        return ret;
    };

    const string str = "ala ma kota";

    for_<maxK + 1>([&] (auto k)
    {
        repeat(nHammingRepeats, [&] {
            set<int> indexList = randNumbersFromRange(0, str.size() - 1, k.value);
            string cur = str;

            for (const int i : indexList)
            {
                cur[i] = 'N';
            }

            for_<k.value>([&] (auto curK)
            {
                REQUIRE(utils::Distance::isHammingAtMostK<curK.value>(cur.c_str(), str.c_str(), cur.size()) == false);
                REQUIRE(utils::Distance::isHammingAtMostK<curK.value>(str.c_str(), cur.c_str(), cur.size()) == false);
            });

            REQUIRE(utils::Distance::isHammingAtMostK<k.value>(str.c_str(), cur.c_str(), cur.size()) == true);
        });
    });
}

} // namespace split_index
