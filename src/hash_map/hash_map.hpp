#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

#include <functional>
#include <string>

#include "../hash_function/hash_functions.hpp"

namespace split_index
{

namespace hash_map
{

class HashMap
{
public:
    HashMap(const std::function<size_t(const char *)> &calcEntrySizeBArg,
            float maxLoadFactorArg,
            int nBucketsHint,
            hash_functions::HashFunctions::HashType hashType);
    virtual ~HashMap() { }

    virtual std::string toString() const = 0;

    /** Clears the hash map, setting new bucket count to [nBucketsHint]. */
    virtual void clear(int nBucketsHint);

    /** Inserts a pair [key] (of size [keySize]) -> [entry]. */
    void insert(const char *key, size_t keySize, char *entry);
    /** Returns a pointer to the entry from pair [key] (of size [keySize]) -> entry. */
    virtual char **retrieve(const char *key, size_t keySize) const = 0;

    /** Returns the total size in bytes, i.e. including both buckets and entries. */
    virtual long calcTotalSizeB() const;

    int getNBuckets() const { return nBuckets; }
    float getCurLoadFactor() const { return curLoadFactor; }
    float getMaxLoadFactor() const { return maxLoadFactor; }

protected:
    void initBuckets();

    void clearBuckets(char **buckets, int nBuckets);
    virtual void clearBucket(char *bucket) = 0;

    virtual void insertEntry(const char *key, size_t keySize, char *entry) = 0;
    virtual void rehash() = 0;

    /** Returns the size of bucket including stored entries in bytes. */
    virtual long calcBucketTotalSizeB(const char *bucket) const = 0;

    /** A hash function used for hashing keys. */
    hash_functions::HashFunctions::HashFunctionType hash;

    /** Returns the size of a stored entry in bytes. */
    std::function<size_t(const char *)> calcEntrySizeB;

    /** Current load factor. */
    float curLoadFactor;
    /** A maximum load factor which causes rehashing when crossed. */
    float maxLoadFactor;

    /** Total number of entries (values). */
    int nEntries = 0;
    /** Total number of buckets (length of buckets array). */
    int nBuckets = 0;

    char **buckets = nullptr;

    /** A factor used for increasing the number of available buckets when rehashing. */
    static constexpr float bucketRehashFactor = 1.5;
};

} // namespace hash_map

} // namespace split_index

#endif // HASH_MAP_HPP
