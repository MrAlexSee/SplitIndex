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
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    REQUIRE(hashMap.getNBuckets() == 5);

    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);
    REQUIRE(hashMap.getMaxLoadFactor() == 1.0f);
}

TEST_CASE("is total size calculation correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    HashMapAligned empty(calcEntrySizeB, 1.0f, 5, hashType);
    REQUIRE(empty.calcTotalSizeB() == sizeof(char **) + 5 * sizeof(char *));

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
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    // This assumes that bucketRehashFactor is set to 2.0f.

    HashMapAligned hashMap(calcEntrySizeB, 0.15f, 5, hashType);
    
    REQUIRE(hashMap.getNBuckets() == 5);
    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);
    REQUIRE(hashMap.getMaxLoadFactor() == 0.15f);

    hashMap.insert("key1", 4, const_cast<char *>(entry.c_str()));

    REQUIRE(hashMap.getNBuckets() == 10);
    REQUIRE(hashMap.getCurLoadFactor() == 0.1f);
    REQUIRE(hashMap.getMaxLoadFactor() == 0.15f);

    hashMap.insert("key2", 4, const_cast<char *>(entry.c_str()));

    REQUIRE(hashMap.getNBuckets() == 20);
    REQUIRE(hashMap.getCurLoadFactor() == 0.1f);
    REQUIRE(hashMap.getMaxLoadFactor() == 0.15f);

    hashMap.insert("key3", 4, const_cast<char *>(entry.c_str()));

    REQUIRE(hashMap.getNBuckets() == 20);
    REQUIRE(hashMap.getCurLoadFactor() == 0.15f);
    REQUIRE(hashMap.getMaxLoadFactor() == 0.15f);
}

TEST_CASE("is copying entry correct", "[hash_map_aligned]")
{
    // We want to include the terminating '\0'.
    auto calcEntrySizeB = [](const char *str) -> size_t { return strlen(str) + 1; };
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    string entry = "entry";
    char *copy1 = HashMapAlignedWhitebox::copyEntry(hashMap, entry.c_str());

    REQUIRE(strcmp(entry.c_str(), copy1) == 0);

    char *copy2 = HashMapAlignedWhitebox::copyEntry(hashMap, "ala ma kota");
    REQUIRE(strcmp("ala ma kota", copy2) == 0);
}

TEST_CASE("is creating a new bucket correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    char *entryStr = const_cast<char *>(entry.c_str());
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    char *bucket = HashMapAlignedWhitebox::createBucket(hashMap, "key1", 4, entryStr);
    string expectedStart = "\4key1";

    REQUIRE(strncmp(bucket, expectedStart.c_str(), expectedStart.size()) == 0);
    REQUIRE(strncmp(*reinterpret_cast<char **>(bucket + expectedStart.size()), entryStr, entry.size()) == 0);
}

TEST_CASE("is adding a single entry to a bucket correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    char *entryStr = const_cast<char *>(entry.c_str());
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    char *bucket = HashMapAlignedWhitebox::createBucket(hashMap, "key1", 4, entryStr);
    HashMapAlignedWhitebox::addToBucket(hashMap, &bucket, "keykey2", 7, entryStr);

    string expected1 = "\4key1";
    string expected2 = "\7keykey2";

    REQUIRE(strncmp(bucket, expected1.c_str(), expected1.size()) == 0);
    REQUIRE(strncmp(*reinterpret_cast<char **>(bucket + expected1.size()), entryStr, entry.size()) == 0);

    bucket += 1 + 4 + sizeof(char **);

    REQUIRE(strncmp(bucket, expected2.c_str(), expected2.size()) == 0);
    REQUIRE(strncmp(*reinterpret_cast<char **>(bucket + expected2.size()), entryStr, entry.size()) == 0);
}

TEST_CASE("is adding multiple entries to a bucket correct", "[hash_map_aligned]")
{
    string entry = "entry";
    auto calcEntrySizeB = [&entry](const char *) -> size_t { return entry.size(); };

    char *entryStr = const_cast<char *>(entry.c_str());
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    string key = "key0";
    char *bucket = HashMapAlignedWhitebox::createBucket(hashMap, key.c_str(), key.size(), entryStr);

    for (int iEntry = 1; iEntry < nEntries; ++iEntry)
    {
        key = "key" + to_string(iEntry);
        // HashMapAlignedWhitebox::addToBucket(hashMap, &bucket, key.c_str(), key.size(), entryStr);

        bucket += iEntry * (1 + key.size() + sizeof(char **));
        string expected = "\4" + key;
        
        // REQUIRE(strncmp(bucket, expected.c_str(), expected.size()) == 0);
        // REQUIRE(strncmp(*reinterpret_cast<char **>(bucket + expected.size()), entryStr, entry.size()) == 0);
    }
}

} // namespace split_index
