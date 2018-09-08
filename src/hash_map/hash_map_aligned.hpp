#ifndef HASH_MAP_ALIGNED
#define HASH_MAP_ALIGNED

#include <string>
#include <utility>
#include <vector>

#include "hash_map.hpp"

class HashMapAligned : public HashMap
{
public:
    HashMapAligned(const std::function<size_t(const char *)> &calcEntrySize,
                   double maxLoadFactor, int sizeHint, const std::string &hashType);
    ~HashMapAligned();

    std::string toString() const override;

    char **get(const char *key, size_t keySize) const override;

protected:
    void rehash() override;
    void insertItem(const char *key, size_t keySize, char *entry) override;

    char *copyEntry(const char *entry);
    char *createBucket(const char *key, size_t keySize, char *entry);
    void addToBucket(int iBucket, const char *key, size_t keySize, char *entry);

    int getNBucketEntries(const char *bucket) const override;

    long getBucketSizeB(const char *bucket) const override;
    long getBucketTotalSizeB(const char *bucket) const override;

    void clearBucket(char *bucket) override;

    static std::string bucketToString(const char *bucket);
};

#endif // HASH_MAP_ALIGNED
