#include "catch.hpp"
#include "repeat.hpp"

#include "../src/hash_map/hash_map_aligned.hpp"

using namespace split_index::hash_map;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;

}

TEST_CASE("is load factor for empty map correct", "[hash_map_aligned]")
{
    auto calcEntrySizeB = [](const char *) -> size_t { return 0; };
    HashMapAligned hashMap(calcEntrySizeB, 1.0f, 5, hashType);

    REQUIRE(hashMap.getCurLoadFactor() == 0.0f);
    REQUIRE(hashMap.getMaxLoadFactor() == 1.0f);
}

} // namespace split_index
