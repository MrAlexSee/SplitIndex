#include <boost/format.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "hash_map.hpp"

using namespace std;

namespace split_index
{

namespace hash_map
{

HashMap::HashMap(const std::function<size_t(const char *)> &calcEntrySizeBArg,
                 double maxLoadFactorArg,
                 int nBucketsHint,
                 const std::string &hashType)
    :calcEntrySizeB(calcEntrySizeBArg),
     maxLoadFactor(maxLoadFactorArg),
     nBuckets(nBucketsHint)
{
    static_assert(bucketRehashFactor > 1.0, "bucket rehash factor must be greater than 1.0");
    assert(nBuckets >= 0);

    initHash(hashType);
    initBuckets();
}

void HashMap::clear(int nBucketsHint)
{
    clearBuckets(buckets, nBuckets);
    curLoadFactor = 0.0;

    nEntries = 0;
    nBuckets = nBucketsHint;
        
    initBuckets();
}

void HashMap::insert(const char *key, size_t keySize, char *entry)
{
    assert(key != nullptr and entry != nullptr);
    assert(keySize > 0 and keySize < 128);

    insertEntry(key, keySize, entry);
    nEntries += 1;

    curLoadFactor = static_cast<double>(nEntries) / nBuckets;

    if (curLoadFactor > maxLoadFactor)
    {
        rehash();
    }
}

long HashMap::calcTotalSizeB() const
{
    long ret = sizeof(char **);
    ret += nBuckets * sizeof(char *);

    for (int i = 0; i < nBuckets; ++i)
    {
        if (buckets[i] != nullptr)
        {
            ret += calcBucketTotalSizeB(buckets[i]);
        }
    }

    return ret;
}

void HashMap::initHash(const std::string &hashType)
{
    using namespace hash_functions;

    if (hashType == "city")
    {
        hash = &HashFunctions::city;
    }
    else if (hashType == "farm")
    {
        hash = &HashFunctions::farm;
    }
    else if (hashType == "farsh")
    {
        hash = &HashFunctions::farsh;
    }
    else if (hashType == "fnv1")
    {
        hash = &HashFunctions::fnv1;
    }
    else if (hashType == "fnv1a")
    {
        hash = &HashFunctions::fnv1a;
    }
    else if (hashType == "murmur3")
    {
        hash = &HashFunctions::murmur3;
    }
    else if (hashType == "sdbm")
    {
        hash = &HashFunctions::sdbm;
    }
    else if (hashType == "spookyv2")
    {
        hash = &HashFunctions::spookyV2;
    }
    else if (hashType == "superfast")
    {
        hash = &HashFunctions::superFast;
    }
    else if (hashType == "xxhash")
    {
        hash = &HashFunctions::xxHash;
    }
    else
    {
        throw runtime_error("bad hash type = " + hashType);
    }
}

void HashMap::initBuckets()
{
    buckets = new char *[nBuckets];

    for (int i = 0; i < nBuckets; ++i)
    {
        buckets[i] = nullptr;
    }
}

void HashMap::clearBuckets(char **buckets, int nBuckets)
{
    if (buckets == nullptr)
    {
        return;
    }

    for (int i = 0; i < nBuckets; ++i)
    {
        if (buckets[i] != nullptr)
        {
            clearBucket(buckets[i]);
        }
    }

    delete[] buckets;
    buckets = nullptr;
}

} // namespace hash_map

} // namespace split_index
