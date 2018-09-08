#include <boost/format.hpp>
#include <iostream>
#include <stdexcept>

#include "hash_map.hpp"

using namespace std;

HashMap::HashMap(const std::function<size_t(const char *)> &calcEntrySize,
                 double maxLoadFactor, int sizeHint, const std::string &hashType)
    :calcEntrySize(calcEntrySize),
     maxLoadFactor(maxLoadFactor),
     nAvailable(sizeHint)
{
    assert(spaceIncrement > 1.0);
    assert(sizeHint >= 0);

    initHash(hashType);
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

void HashMap::initHash(const std::string &hashType)
{
    if (hashType == "city")
    {
        hash = &HashFunctions::F_City;
    }
    else if (hashType == "farm")
    {
        hash = &HashFunctions::F_Farm;
    }
    else if (hashType == "farsh")
    {
        hash = &HashFunctions::F_farsh;
    }
    else if (hashType == "fnv1")
    {
        hash = &HashFunctions::F_FNV1;
    }
    else if (hashType == "fnv1a")
    {
        hash = &HashFunctions::F_FNV1a;
    }
    else if (hashType == "murmur3")
    {
        hash = &HashFunctions::F_Murmur3;
    }
    else if (hashType == "sdbm")
    {
        hash = &HashFunctions::F_sdbm;
    }
    else if (hashType == "spookyv2")
    {
        hash = &HashFunctions::F_SpookyV2;
    }
    else if (hashType == "superfast")
    {
        hash = &HashFunctions::F_SuperFast;
    }
    else if (hashType == "xxhash")
    {
        hash = &HashFunctions::F_xxHash;
    }
    else
    {
        throw invalid_argument("bad hash type = " + hashType);
    }
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
