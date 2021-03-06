#ifndef HASH_MAP_ALIGNED_HPP
#define HASH_MAP_ALIGNED_HPP

#include <utility>
#include <vector>

#include "hash_map.hpp"

#ifndef HASH_MAP_ALIGNED_WHITEBOX
#define HASH_MAP_ALIGNED_WHITEBOX
#endif

namespace split_index
{

namespace hash_map
{

/** This is an aligned version of a map.
 * It stores keys in a single bucket contiguously for better cache utilization. */
class HashMapAligned : public HashMap
{
public:
    HashMapAligned(const std::function<size_t(const char *)> &calcEntrySizeB,
        float maxLoadFactor,
        int nBucketsHint,
        hash_functions::HashFunctions::HashType hashType);
    ~HashMapAligned() override;

    std::string toString() const override;

    char **retrieve(const char *key, size_t keySize) const override;

protected:
    void clearBucket(char *bucket) override;

    void insertEntry(const char *key, size_t keySize, char *entry) override;
    void rehash() override;

    /** Returns the size of bucket including stored entries in bytes. */
    long calcBucketTotalSizeB(const char *bucket) const override;
    /** Returns the size of bucket excluding stored entries in bytes. */
    long calcBucketSizeB(const char *bucket) const;

    /** Returns a deep copy of the entry. */
    char *copyEntry(const char *entry) const;
    
    /** Returns a new bucket which already contains a pair [key] -> [entry]. */
    char *createBucket(const char *key, size_t keySize, char *entry) const;
    /** Adds a pair [key] -> [entry] to [bucket]. Resizes the bucket as appropriate. */
    void addToBucket(char **bucket, const char *key, size_t keySize, char *entry);

    HASH_MAP_ALIGNED_WHITEBOX
};

} // namespace hash_map

} // namespace split_index

#endif // HASH_MAP_ALIGNED_HPP
