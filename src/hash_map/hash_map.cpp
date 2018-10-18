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
                 float maxLoadFactorArg,
                 int nBucketsHint,
                 hash_functions::HashFunctions::HashType hashType)
    :calcEntrySizeB(calcEntrySizeBArg),
     maxLoadFactor(maxLoadFactorArg),
     nBuckets(nBucketsHint)
{
    static_assert(bucketRehashFactor > 1.0, "bucket rehash factor must be greater than 1.0");
assert(maxLoadFactor > 0.0);
    assert(nBuckets >= 0);

    hash = hash_functions::HashFunctions::getHashFunction(hashType);
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

    curLoadFactor = static_cast<float>(nEntries) / nBuckets;

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
