#ifndef HASH_MAP
#define HASH_MAP

#include <functional>
#include <string>

#include "../hash_function/hash_functions.hpp"

#include "hash_map.hpp"

class HashMap
{
public:
    HashMap(const std::function<size_t(const char *)> &calcEntrySize,
            double maxLoadFactor, int sizeHint, const std::string &hashType);
    virtual ~HashMap() { }

    virtual std::string toString() const = 0;

    void insert(const char *key, size_t keySize, char *entry);
    virtual char **get(const char *key, size_t keySize) const = 0;

    virtual void clear(int sizeHint);

    virtual int getNEntries() const; // Number of entries.
    virtual long getTotalSizeB() const; // Total size (including the entries) in bytes.

    int getNBuckets() const { return nBuckets; }
    double getLoadFactor() const { return curLoadFactor; }
    double getMaxLoadFactor() const { return maxLoadFactor; }

protected:
    void initHash(const std::string &hashType);
    void initBuckets();

    virtual void rehash() = 0;
    virtual void insertItem(const char *key, size_t keySize, char *entry) = 0;

    virtual int getNBucketEntries(const char *bucket) const = 0;
    
    virtual long getBucketSizeB(const char *bucket) const = 0; // Only the bucket.
    virtual long getBucketTotalSizeB(const char *bucket) const = 0; // Including entry sizes.

    void clearBuckets(char **buckets, int size);
    virtual void clearBucket(char *bucket) = 0;

    HashFunctions::HashFunctionType hash;
    std::function<size_t(const char *)> calcEntrySize;

    double curLoadFactor;
    double maxLoadFactor;

    int nEntries   = 0;
    int nBuckets   = 0;
    int nAvailable = 0;

    char **buckets = nullptr;

    static constexpr double spaceIncrement = 1.2;
};

#endif // HASH_MAP
