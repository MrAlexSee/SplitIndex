#include <boost/format.hpp>
#include <iostream>

#include "../main/params.hpp"

#include "hash_map.hpp"

using namespace std;

HashMap::HashMap(const std::function<size_t(const char *)> &calcEntrySize,
                 double maxLoadFactor, int sizeHint)
    :calcEntrySize(calcEntrySize),
     maxLoadFactor(maxLoadFactor),
     nAvailable(sizeHint)
{
    assert(spaceIncrement > 1.0);
    assert(sizeHint >= 0);

    initHash();
    initBuckets();
}

void HashMap::insert(const char *key, size_t keySize, char *entry)
{
    assert(key != nullptr and entry != nullptr);

    insertItem(key, keySize, entry);
    nEntries += 1;

    curLoadFactor = static_cast<double>(nEntries) / nAvailable;

    if (curLoadFactor > maxLoadFactor)
    {
        rehash();
    }
}

void HashMap::clear(int sizeHint)
{
    clearBuckets(buckets, nAvailable);

    nEntries = 0;
    nBuckets = 0;
    curLoadFactor = 0.0;
    
    nAvailable = sizeHint;
    initBuckets();
}

int HashMap::getNEntries() const
{
    int total = 0;

    for (int i = 0; i < nAvailable; ++i)
    {
        if (buckets[i] != nullptr)
        {
            total += getNBucketEntries(buckets[i]);
        }
    }

    return total;
}

long HashMap::getTotalSizeB() const
{
    long total = nAvailable * sizeof(char *);

    for (int i = 0; i < nAvailable; ++i)
    {
        if (buckets[i] != nullptr)
        {
            total += getBucketTotalSizeB(buckets[i]);
        }
    }

    return total;
}

void HashMap::initHash()
{
#if HASH_TYPE == 0
    hash = &HashFunctions::F_City;
#elif HASH_TYPE == 1
    hash = &HashFunctions::F_FNV1;
#elif HASH_TYPE == 2
    hash = &HashFunctions::F_FNV1a;
#elif HASH_TYPE == 3
    hash = &HashFunctions::F_Murmur3;
#elif HASH_TYPE == 4
    hash = &HashFunctions::F_sdbm;
#elif HASH_TYPE == 5
    hash = &HashFunctions::F_SpookyV2;
#elif HASH_TYPE == 6
    hash = &HashFunctions::F_SuperFast;
#elif HASH_TYPE == 7
    hash = &HashFunctions::F_xxHash;
#elif HASH_TYPE == 8
    hash = &HashFunctions::F_farsh;
#elif HASH_TYPE == 9
    hash = &HashFunctions::F_Farm;
#else
    #error Bad HASH_TYPE
#endif 
}

void HashMap::initBuckets()
{
    buckets = new char *[nAvailable];

    for (int i = 0; i < nAvailable; ++i)
    {
        buckets[i] = nullptr;
    }
}

void HashMap::clearBuckets(char **buckets, int size)
{
    if (buckets == nullptr)
        return;

    for (int i = 0; i < size; ++i)
    {
        if (buckets[i] != nullptr)
        {
            clearBucket(buckets[i]);
        }

        free(buckets[i]);
    }

    delete[] buckets;
    buckets = nullptr;
}
