#include <boost/format.hpp>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>

#include "hash_map_aligned.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace hash_map
{

HashMapAligned::HashMapAligned(const std::function<size_t(const char *)> &calcEntrySizeB,
        float maxLoadFactor,
        int nBucketsHint,
        hash_functions::HashFunctions::HashType hashType)
    :HashMap(calcEntrySizeB,
        maxLoadFactor,
        nBucketsHint,
        hashType)
{ }

HashMapAligned::~HashMapAligned()
{
    clearBuckets(buckets, nBuckets);
}

string HashMapAligned::toString() const
{
    const float totalSizeKB = calcTotalSizeB() / 1024.0f;
    
    const float avgBucketSize = static_cast<float>(nEntries) / nBuckets;
    const string formatStr = "Hash map: %1% entries, LF = %2% (max = %3%), total size = %4% KB, avg bucket size = %5%";

    return (boost::format(formatStr) % nEntries % curLoadFactor % maxLoadFactor 
        % totalSizeKB % avgBucketSize).str();
}

char **HashMapAligned::retrieve(const char *key, size_t keySize) const
{
    assert(keySize > 0 and keySize < 128);
    const size_t index = hash(key, keySize) % nBuckets;

    if (buckets[index] == nullptr)
    {
        return nullptr;
    }

    char *bucket = buckets[index];

    while (*bucket != 0)
    {
        const size_t keyInBucketSize = *bucket;

        if (keySize == keyInBucketSize)
        {
            if (memcmp(bucket + 1, key, keySize) == 0)
            {
                return reinterpret_cast<char **>(bucket + 1 + keyInBucketSize);
            }
        }

        bucket += 1 + keyInBucketSize + sizeof(char *);
    }

    return nullptr;
}

void HashMapAligned::clearBucket(char *bucket)
{
    assert(bucket != nullptr);
    char *it = bucket;

    while (*it != 0)
    {
        const size_t keyInBucketSize = *it;

        free(*reinterpret_cast<void **>(it + 1 + keyInBucketSize));
        it += 1 + keyInBucketSize + sizeof(char *);
    }

    free(bucket);
}

void HashMapAligned::insertEntry(const char *key, size_t keySize, char *entry)
{
    const size_t index = hash(key, keySize) % nBuckets;
    char *newEntry = copyEntry(entry);

    if (buckets[index] == nullptr)
    {
        buckets[index] = createBucket(key, keySize, newEntry);
    }
    else
    {
        addToBucket(buckets + index, key, keySize, newEntry);
    }
}

void HashMapAligned::rehash()
{
    assert(curLoadFactor > maxLoadFactor);

    char **oldBuckets = buckets;
    const int oldNBuckets = nBuckets;

    while (curLoadFactor > maxLoadFactor)
    {
        nBuckets *= bucketRehashFactor;
        curLoadFactor = static_cast<float>(nEntries) / nBuckets;
    }

    initBuckets();

    for (int i = 0; i < oldNBuckets; ++i)
    {
        if (oldBuckets[i] != nullptr)
        {
            char *bucket = oldBuckets[i];

            while (*bucket != 0)
            {
                const size_t keyInBucketSize = *bucket;

                insertEntry(bucket + 1, keyInBucketSize, *reinterpret_cast<char **>(bucket + 1 + keyInBucketSize));
                bucket += 1 + keyInBucketSize + sizeof(char *);
            }
        }
    }
    
    clearBuckets(oldBuckets, oldNBuckets);
}

long HashMapAligned::calcBucketTotalSizeB(const char *bucket) const
{
    long ret = 0;
    const char *start = bucket;

    while (*bucket != 0)
    {
        const size_t keyInBucketSize = *bucket;

        ret += calcEntrySizeB(*reinterpret_cast<const char * const*>(bucket + 1 + keyInBucketSize));
        bucket += 1 + keyInBucketSize + sizeof(char *);
    }

    ret += (bucket - start + 1); // Includes the terminating 0.
    return ret;
}

long HashMapAligned::calcBucketSizeB(const char *bucket) const
{
    const char *start = bucket;

    while (*bucket != 0)
    {
        bucket += 1 + *bucket + sizeof(char *);
    }

    return (bucket - start + 1); // Includes the terminating 0.
}

char *HashMapAligned::copyEntry(const char *entry) const
{
    // We do not use strdup here because there might be zeros inside the entry.
    const size_t entrySize = calcEntrySizeB(entry);
    char *newEntry = static_cast<char *>(malloc(entrySize));

    memcpy(newEntry, entry, entrySize);
    return newEntry;
}

char *HashMapAligned::createBucket(const char *key, size_t keySize, char *entry) const
{
    const size_t bucketSize = 1 + keySize + sizeof(char *) + 1;
    char *bucket = static_cast<char *>(malloc(bucketSize));

    *bucket = keySize;
    memcpy(bucket + 1, key, keySize);

    *reinterpret_cast<char **>(bucket + 1 + keySize) = entry;
    bucket[bucketSize - 1] = 0;

    assert(bucketSize == 2 + keySize + sizeof(char *));
    return bucket;
}

void HashMapAligned::addToBucket(char **bucket, const char *key, size_t keySize, char *entry)
{
    const size_t oldSize = calcBucketSizeB(*bucket);
    const size_t newSize = oldSize + keySize + sizeof(char *) + 1; // This includes the terminating 0.

    void *ptr = realloc(*bucket, newSize);
    *bucket = static_cast<char *>(ptr);
    
    (*bucket)[oldSize - 1] = keySize;
    memcpy(*bucket + oldSize, key, keySize);

    *reinterpret_cast<char **>(*bucket + oldSize + keySize) = entry;
    (*bucket)[newSize - 1] = 0;
}

} // namespace hash_map

} // namespace split_index
