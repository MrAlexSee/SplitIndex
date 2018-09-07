#include <boost/format.hpp>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "../utils/helpers.hpp"
#include "hash_map_aligned.hpp"

using namespace split_index;
using namespace std;

HashMapAligned::HashMapAligned(const std::function<size_t(const char *)> &calcEntrySize,
                               double maxLoadFactor, int sizeHint)
    :HashMap(calcEntrySize, maxLoadFactor, sizeHint)
{

}

HashMapAligned::~HashMapAligned()
{
    clearBuckets(this->buckets, this->nAvailable);
}

string HashMapAligned::toString() const
{
    double totalSizeKB = this->getTotalSizeB() / 1024.0;
    totalSizeKB = utils::Helpers::round2Places(totalSizeKB);
    
    double avgBucketSize = static_cast<double>(getNEntries()) / nBuckets;
    string formatStr = "Hash map: %2% entries, lf = %3% (max = %4%), total size = %5% KB, avg bucket size = %6%";

    return (boost::format(formatStr)
            % getNEntries() % curLoadFactor % maxLoadFactor % totalSizeKB % avgBucketSize).str();
}

char **HashMapAligned::get(const char *key, size_t keySize) const
{
    size_t index = hash(key, keySize) % nAvailable;

    if (buckets[index] == nullptr)
        return nullptr;

    char *bucket = buckets[index];

    // We traverse the bucket.
    while (bucket[0] != 0)
    {
        if (static_cast<size_t>(bucket[0]) == keySize)
        {
            if (strncmp(bucket + 1, key, keySize) == 0)
            {
                // We return the pointer to the value.
                return reinterpret_cast<char **>(bucket + 1 + bucket[0]);
            }
        }

        bucket += 1 + bucket[0] + sizeof(char *);
    }

    return nullptr;
}

void HashMapAligned::rehash()
{
    char **oldBuckets = buckets;
    int oldNAvailable = nAvailable;

    nAvailable *= spaceIncrement;
    initBuckets();

    nBuckets = 0;

    for (int i = 0; i < oldNAvailable; ++i)
    {
        if (oldBuckets[i] != nullptr)
        {
            char *bucket = oldBuckets[i];

            while (bucket[0] != 0)
            {
                insertItem(bucket + 1, bucket[0], *reinterpret_cast<char **>(bucket + 1 + bucket[0]));
                bucket += 1 + bucket[0] + sizeof(char *);
            }
        }
    }
    
    clearBuckets(oldBuckets, oldNAvailable);
    curLoadFactor = static_cast<double>(nBuckets) / nAvailable;

    if (curLoadFactor >= maxLoadFactor)
    {
        rehash();
    }
}

void HashMapAligned::insertItem(const char *key, size_t keySize, char *entry)
{
    size_t index = hash(key, keySize) % nAvailable;
    char *newEntry = copyEntry(entry);

    if (buckets[index] == nullptr)
    {
        buckets[index] = createBucket(key, keySize, newEntry);
        nBuckets += 1;
    }
    else
    {
        addToBucket(index, key, keySize, newEntry);
    }
}

char *HashMapAligned::copyEntry(const char *entry)
{
    // Don't use strdup here because there might be zeros inside the entry.
    size_t entrySize = calcEntrySize(entry);
    char *newEntry = static_cast<char *>(malloc(entrySize));

    for (size_t i = 0; i < entrySize; ++i)
    {
        newEntry[i] = entry[i];
    }

    assert(newEntry[entrySize - 1] == '\0');
    return newEntry;
}

char *HashMapAligned::createBucket(const char *key, size_t keySize, char *entry)
{
    size_t newSize = 1 + keySize + sizeof(char *) + 1;
    char *bucket = static_cast<char *>(malloc(newSize));

    bucket[0] = keySize;

    for (size_t i = 0; i < keySize; ++i)
    {
        bucket[1 + i] = key[i];
    }

    *reinterpret_cast<char **>(bucket + 1 + keySize) = entry;
    bucket[newSize - 1] = '\0';

    assert(newSize - 1 == 1 + keySize + sizeof(char *));
    return bucket;
}

void HashMapAligned::addToBucket(int iBucket, const char *key, size_t keySize, char *entry)
{
    size_t oldSize = getBucketSizeB(buckets[iBucket]); // This includes the terminating '\0'.
    size_t newSize = oldSize + keySize + sizeof(char *) + 1;

    buckets[iBucket] = static_cast<char *>(realloc(buckets[iBucket], newSize));

    buckets[iBucket][oldSize - 1] = keySize;

    for (size_t i = 0; i < keySize; ++i)
    {
        buckets[iBucket][oldSize + i] = key[i];
    }

    *reinterpret_cast<char **>(buckets[iBucket] + oldSize + keySize) = entry;
    buckets[iBucket][newSize - 1] = '\0';
}

int HashMapAligned::getNBucketEntries(const char *bucket) const
{
    int total = 0;

    while (bucket[0] != 0)
    {                
        total += 1;
        bucket += 1 + bucket[0] + sizeof(char *);
    }

    return total;
}

long HashMapAligned::getBucketSizeB(const char *bucket) const
{
    const char *start = bucket;

    while (bucket[0] != 0)
    {
        bucket += 1 + bucket[0] + sizeof(char *);
    }

    return (bucket - start + 1); // Including the terminating '\0'.
}

long HashMapAligned::getBucketTotalSizeB(const char *bucket) const
{
    long total = 0;
    const char *start = bucket;

    while (bucket[0] != 0)
    {
        total += calcEntrySize(*reinterpret_cast<const char * const*>(bucket + 1 + bucket[0]));
        bucket += 1 + bucket[0] + sizeof(char *);
    }

    total += (bucket - start + 1); // Including the terminating '\0'.
    return total;
}

void HashMapAligned::clearBucket(char *bucket)
{
    assert(bucket != nullptr);

    while (bucket[0] != 0)
    {
        free(*reinterpret_cast<void **>(bucket + 1 + bucket[0]));
        bucket += 1 + bucket[0] + sizeof(char *);
    }
}

string HashMapAligned::bucketToString(const char *bucket)
{
    string out = "|";

    while (bucket[0] != 0)
    {
        out += to_string(static_cast<int>(bucket[0]));

        for (size_t i = 0; i < static_cast<size_t>(bucket[0]); ++i)
        {
            out += bucket[i + 1];
        }

        out += "P";
        bucket += 1 + bucket[0] + sizeof(char *);
    }

    return out + "|";
}
