#include <cstring>

#include "catch.hpp"
#include "repeat.hpp"

#include "hash_map_aligned_whitebox.hpp"

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
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned empty(calcEntrySizeB, 1.0f, 0, hashType);
    REQUIRE(empty.calcTotalSizeB() == sizeof(char **));

    const int nBuckets = 5000;
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);

    hashMap.insert("key1", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key2", 4, const_cast<char *>(entry.c_str()));
    hashMap.insert("key3", 4, const_cast<char *>(entry.c_str()));

    const long totalBucketSize = 1 + 4 + sizeof(char *) + entry.size() + 1;
    REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **) + nBuckets * sizeof(char *) + 3 * totalBucketSize);
}

TEST_CASE("is clearing correct for 0 buckets hint", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

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
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    const int nBuckets = 5;
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);

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
    const int nBuckets = 500;

    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned hashMap(calcEntrySizeB, 1.0f, nBuckets, hashType);
    long totalBucketsSize = 0;

    for (const string &key : { "key1", "1", "123", "ala ma kota" })
    {
        hashMap.insert(key.c_str(), key.size(), const_cast<char *>(entry.c_str()));
        totalBucketsSize += 1 + key.size() + sizeof(char *) + entry.size() + 1;

        repeat(nReadRepeats, [&] {
            char **fromHashMap = hashMap.retrieve(key.c_str(), key.size());
            REQUIRE(strncmp(*fromHashMap, entry.c_str(), entry.size()) == 0);

            REQUIRE(hashMap.calcTotalSizeB() == sizeof(char **) + nBuckets * sizeof(char *) + totalBucketsSize);
        });
    }
}

TEST_CASE("is inserting and retrieving multiple entries correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5000, hashType);

    for (int iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        const string key = "key" + to_string(iEntry);
        hashMap.insert(key.c_str(), key.size(), const_cast<char *>(entry.c_str()));
    }

    repeat(nReadRepeats, [&] {
        for (int iEntry = 0; iEntry < nEntries; ++iEntry)
        {
            const string key = "key" + to_string(iEntry);

            char **fromHashMap = hashMap.retrieve(key.c_str(), key.size());
            REQUIRE(strncmp(*fromHashMap, entry.c_str(), entry.size()) == 0);
        }
    });
}

TEST_CASE("is rehashing correct", "[hash_map_aligned]")
{

}

TEST_CASE("is copying entry correct", "[hash_map_aligned]")
{

}

TEST_CASE("is creating a new bucket correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 0, hashType);
    char *entryStr = const_cast<char *>(entry.c_str());

    char *bucket = HashMapAlignedWhitebox::createBucket(hashMap, "key1", 4, entryStr);
    string expectedStart = "\4key1";

    REQUIRE(strncmp(bucket, expectedStart.c_str(), expectedStart.size()) == 0);
    REQUIRE(strncmp(*reinterpret_cast<char **>(bucket + expectedStart.size()), entryStr, entry.size()) == 0);
}

TEST_CASE("is adding to a bucket correct", "[hash_map_aligned]")
{

}

} // namespace split_index
