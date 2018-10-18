#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "../src/hash_map/hash_map_aligned.hpp"

using namespace split_index::hash_map;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;

constexpr int nReadRepeats = 10;
constexpr int nEntries = 10;

}

TEST_CASE("is empty map correctly initialized", "[hash_map_aligned]")
{
    auto calcEntrySizeB = [](const char *) -> size_t { return 0; };
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 0, hashType);

    REQUIRE(hashMap.getNBuckets() == 0);

    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);
    REQUIRE(hashMap.getMaxLoadFactor() == 1.0f);
}

TEST_CASE("is total size calculation correct", "[hash_map_aligned]")
{
    auto calcEntrySizeB = [](const char *) -> size_t { return 5; };

    HashMapAligned empty(calcEntrySizeB, 1.0f, 0, hashType);
    REQUIRE(empty.calcTotalSizeB() == sizeof(char **));

    const int nBuckets = 5000;
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);

    string entry = "entry";

    hashMap.insert("key1", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key2", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key3", 4, const_cast<char *>(entry.c_str()));

    const long totalBucketSize = 1 + 4 + sizeof(char *) + 5 + 1;
    REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **) + nBuckets * sizeof(char *) + 3 * totalBucketSize);
}

TEST_CASE("is clearing correct for 0 buckets hint", "[hash_map_aligned]")
{
    auto calcEntrySizeB = [](const char *) -> size_t { return 5; };
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    string entry = "entry";

    hashMap.insert("key1", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key2", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key3", 4, const_cast<char *>(entry.c_str()));

    REQUIRE(hashMap.getNBuckets() == 5);

    hashMap.clear(0);

    REQUIRE(hashMap.getNBuckets() == 0);
    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);

    REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **));
}

TEST_CASE("is clearing correct for 5 buckets hint", "[hash_map_aligned]")
{
    const int nBuckets = 5;

    auto calcEntrySizeB = [](const char *) -> size_t { return 5; };
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);

    string entry = "entry";

    hashMap.insert("key1", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key2", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key3", 4, const_cast<char *>(entry.c_str()));

    REQUIRE(hashMap.getNBuckets() == nBuckets);

    hashMap.clear(nBuckets);

    REQUIRE(hashMap.getNBuckets() == nBuckets);
    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);

    REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **) + nBuckets * sizeof(char *));
}

TEST_CASE("is inserting and retrieving a single entry correct", "[hash_map_aligned]")
{
    const int nBuckets = 5;

    for (const string &key : { "key1", "1", "123", "ala ma kota" })
    {
        repeat(nReadRepeats, [&key] {
            auto calcEntrySizeB = [](const char *) -> size_t { return 5; };
            HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);

            string entry = "entry";

            hashMap.insert(key.c_str(), key.size(), const_cast<char *>(entry.c_str()));

            char **fromHashMap = hashMap.retrieve(key.c_str(), key.size());
            REQUIRE(strcmp(*fromHashMap, entry.c_str()) == 0);

            const long totalBucketSize = 1 + key.size() + sizeof(char *) + 5 + 1;
            REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **) + nBuckets * sizeof(char *) + totalBucketSize);
        });
    }
}

TEST_CASE("is inserting and retrieving multiple entries correct", "[hash_map_aligned]")
{
    repeat(nReadRepeats, [] {
        auto calcEntrySizeB = [](const char *) -> size_t { return 5; };
        HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5000, hashType);

        string entry = "entry";

        for (int iEntry = 0; iEntry < nEntries; ++iEntry)
        {
            const string key = "key" + to_string(iEntry);
            hashMap.insert(key.c_str(), key.size(), const_cast<char *>(entry.c_str()));
        }

        for (int iEntry = 0; iEntry < nEntries; ++iEntry)
        {
            const string key = "key" + to_string(iEntry);

            char **fromHashMap = hashMap.retrieve(key.c_str(), key.size());
            REQUIRE(strcmp(*fromHashMap, entry.c_str()) == 0);
        }
    });
}

} // namespace split_index
