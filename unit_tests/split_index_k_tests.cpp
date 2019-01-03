#include <cstring>
#include <string>

#include "catch.hpp"
#include "repeat.hpp"

#include "split_index_k_whitebox.hpp"

#include "../src/index/split_index_k.hpp"

using namespace split_index;
using namespace std;

namespace split_index
{

namespace
{

hash_functions::HashFunctions::HashType hashType = hash_functions::HashFunctions::HashType::XxHash;
constexpr size_t maxK = 3;

}

TEST_CASE("does split index k throw for bad k", "[split_index_k]")
{
    REQUIRE_THROWS(SplitIndexK<0>({ }, hashType, 1.0f));
    REQUIRE_THROWS(SplitIndexK<100>({ }, hashType, 1.0f));
}

TEST_CASE("does split index k throw for empty words", "[split_index_k]")
{
    for_<maxK + 1>([&] (auto k)
    {
        if (k.value > 0)
        {
            REQUIRE_THROWS(SplitIndexK<k.value>({ }, hashType, 1.0f));
        }
    });
}

TEST_CASE("is k entry size calculation correct", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    char *entry = SplitIndexKWhitebox::createEntry<1>("ala", 3, 0);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 8);

    // 4 words in total -- no additional part bytes added.

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "ada", 3, 0);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 12);

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "index", 5, 1);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 18);

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "pies", 4, 1);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 23);

    // 5 words in total -- one additional part byte added.

    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "smok", 4, 0);
    REQUIRE(SplitIndexKWhitebox::calcEntrySizeB(indexk1, entry) == 29);
}

TEST_CASE("is k entry word count calculation correct", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    char *entry = SplitIndexKWhitebox::createEntry<1>("ala", 3, 0);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "ada", 3, 0);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "index", 5, 1);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "pies", 4, 1);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry, "smok", 4, 0);

    REQUIRE(SplitIndexKWhitebox::calcEntryNWords<1>(entry) == 5);
}

TEST_CASE("is storing word parts in buffers correct for even size for k = 1", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);
    indexk1.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk1, "pies");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk1);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk1);

    REQUIRE(wordPartSizes[0] == 2);
    REQUIRE(wordPartSizes[1] == 2);

    REQUIRE(memcmp(wordPartBuf[0], "pi", 2) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "es", 2) == 0);
}

TEST_CASE("is storing word parts in buffers correct for even size for k = 2", "[split_index_k]")
{
    SplitIndexK<2> indexk2({ "index" }, hashType, 1.0f);
    indexk2.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk2, "tyrada");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk2);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk2);

    REQUIRE(wordPartSizes[0] == 2);
    REQUIRE(wordPartSizes[1] == 2);
    REQUIRE(wordPartSizes[2] == 2);

    REQUIRE(memcmp(wordPartBuf[0], "ty", 2) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "ra", 2) == 0);
    REQUIRE(memcmp(wordPartBuf[2], "da", 2) == 0);
}

TEST_CASE("is storing word parts in buffers correct for even size for k = 3", "[split_index_k]")
{
    SplitIndexK<3> indexk3({ "index" }, hashType, 1.0f);
    indexk3.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk3, "tyrada");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk3);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk3);

    REQUIRE(wordPartSizes[0] == 1);
    REQUIRE(wordPartSizes[1] == 1);
    REQUIRE(wordPartSizes[2] == 1);
    REQUIRE(wordPartSizes[3] == 3);

    REQUIRE(memcmp(wordPartBuf[0], "t", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "y", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[2], "r", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[3], "ada", 3) == 0);
}

TEST_CASE("is storing word parts in buffers correct for odd size for k = 1", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);
    indexk1.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk1, "piesa");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk1);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk1);

    REQUIRE(wordPartSizes[0] == 2);
    REQUIRE(wordPartSizes[1] == 3);

    REQUIRE(memcmp(wordPartBuf[0], "pi", 2) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "esa", 3) == 0);
}

TEST_CASE("is storing word parts in buffers correct for odd size for k = 2", "[split_index_k]")
{
    SplitIndexK<2> indexk2({ "index" }, hashType, 1.0f);
    indexk2.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk2, "owocami");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk2);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk2);

    REQUIRE(wordPartSizes[0] == 2);
    REQUIRE(wordPartSizes[1] == 2);
    REQUIRE(wordPartSizes[2] == 3);

    REQUIRE(memcmp(wordPartBuf[0], "ow", 2) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "oc", 3) == 0);
    REQUIRE(memcmp(wordPartBuf[2], "ami", 3) == 0);
}

TEST_CASE("is storing word parts in buffers correct for odd size for k = 3", "[split_index_k]")
{
    SplitIndexK<3> indexk3({ "index" }, hashType, 1.0f);
    indexk3.construct();

    SplitIndexKWhitebox::storeWordPartsInBuffers(indexk3, "owocami");

    const size_t *wordPartSizes = SplitIndexKWhitebox::getWordPartSizes(indexk3);
    const char *const *wordPartBuf = SplitIndexKWhitebox::getWordPartBuf(indexk3);

    REQUIRE(wordPartSizes[0] == 1);
    REQUIRE(wordPartSizes[1] == 1);
    REQUIRE(wordPartSizes[2] == 1);
    REQUIRE(wordPartSizes[3] == 4);

    REQUIRE(memcmp(wordPartBuf[0], "o", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[1], "w", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[2], "o", 1) == 0);
    REQUIRE(memcmp(wordPartBuf[3], "cami", 4) == 0);
}

TEST_CASE("is part size calculation correct", "[split_index_k]")
{    
    REQUIRE(SplitIndexKWhitebox::getPartSize<1>(4) == 2);
    REQUIRE(SplitIndexKWhitebox::getPartSize<2>(4) == 1);

    REQUIRE(SplitIndexKWhitebox::getPartSize<1>(10) == 5);
    REQUIRE(SplitIndexKWhitebox::getPartSize<2>(10) == 3);
}

TEST_CASE("is creating entry correct for k = 1", "[split_index_k]")
{
    // Word = tyradami
    char *entry1 = SplitIndexKWhitebox::createEntry<1>("tyra", 4, 0);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);
    REQUIRE(entry1[2] == 0x0u);
    REQUIRE(memcmp(entry1 + 3, "\4tyra\0", 6) == 0);

    char *entry2 = SplitIndexKWhitebox::createEntry<1>("dami", 4, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry2) == 1u);
    REQUIRE(entry2[2] == 0x1u);
    REQUIRE(memcmp(entry2 + 3, "\4dami\0", 6) == 0);
}

TEST_CASE("is creating entry correct for k = 2", "[split_index_k]")
{
    // Word = tyradami
    char *entry1 = SplitIndexKWhitebox::createEntry<2>("ty", 2, 0);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);
    REQUIRE(entry1[2] == 0x0u);
    REQUIRE(memcmp(entry1 + 3, "\2ty\0", 4) == 0);

    char *entry2 = SplitIndexKWhitebox::createEntry<2>("ra", 2, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry2) == 1u);
    REQUIRE(entry2[2] == 0x1u);
    REQUIRE(memcmp(entry2 + 3, "\2ra\0", 4) == 0);

    char *entry3 = SplitIndexKWhitebox::createEntry<2>("dami", 4, 2);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry3) == 1u);
    REQUIRE(entry3[2] == 0x2u);
    REQUIRE(memcmp(entry3 + 3, "\4dami\0", 6) == 0);
}

TEST_CASE("is creating entry correct for k = 3", "[split_index_k]")
{
    // Word = tyradami
    char *entry1 = SplitIndexKWhitebox::createEntry<3>("ty", 2, 0);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);
    REQUIRE(entry1[2] == 0x0u);
    REQUIRE(memcmp(entry1 + 3, "\2ty\0", 4) == 0);

    char *entry2 = SplitIndexKWhitebox::createEntry<3>("ra", 2, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry2) == 1u);
    REQUIRE(entry2[2] == 0x1u);
    REQUIRE(memcmp(entry2 + 3, "\2ra\0", 4) == 0);

    char *entry3 = SplitIndexKWhitebox::createEntry<3>("da", 2, 2);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry3) == 1u);
    REQUIRE(entry3[2] == 0x2u);
    REQUIRE(memcmp(entry3 + 3, "\2da\0", 6) == 0);

    char *entry4 = SplitIndexKWhitebox::createEntry<3>("mi", 2, 3);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry4) == 1u);
    REQUIRE(entry4[2] == 0x3u);
    REQUIRE(memcmp(entry4 + 3, "\2mi\0", 4) == 0);
}

TEST_CASE("is adding to entry correct for k = 1", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    // Word = psami
    char *entry1 = SplitIndexKWhitebox::createEntry<1>("ps", 2, 0);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry1, "ami", 3, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);

    REQUIRE(entry1[2] == 0b00000100);
    REQUIRE(memcmp(entry1 + 3, "\2ps\3ami\0", 8) == 0);
}

TEST_CASE("is adding to entry correct for k = 2", "[split_index_k]")
{
    SplitIndexK<2> indexk2({ "index" }, hashType, 1.0f);

    // Word = tyradami
    char *entry1 = SplitIndexKWhitebox::createEntry<2>("ty", 2, 0);

    SplitIndexKWhitebox::addToEntry(indexk2, &entry1, "ra", 2, 1);
    SplitIndexKWhitebox::addToEntry(indexk2, &entry1, "dami", 4, 2);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);
    REQUIRE(entry1[2] == 0b00100100);
    REQUIRE(memcmp(entry1 + 3, "\2ty\2ra\4dami\0", 12) == 0);
}

TEST_CASE("is adding to entry correct for k = 3", "[split_index_k]")
{
    SplitIndexK<3> indexk3({ "index" }, hashType, 1.0f);

    // Word = tyradami
    char *entry1 = SplitIndexKWhitebox::createEntry<3>("ty", 2, 0);

    SplitIndexKWhitebox::addToEntry(indexk3, &entry1, "ra", 2, 1);
    SplitIndexKWhitebox::addToEntry(indexk3, &entry1, "da", 2, 2);
    SplitIndexKWhitebox::addToEntry(indexk3, &entry1, "mi", 2, 3);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry1) == 1u);
    REQUIRE(entry1[2] == 0b11100100);
    REQUIRE(memcmp(entry1 + 3, "\2ty\2ra\2da\2mi\0", 13) == 0);
}

TEST_CASE("is trying match part correct empty for k = 1", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    // Word = psami
    char *entry1 = SplitIndexKWhitebox::createEntry<1>("ps", 2, 0);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry1, "ami", 3, 1);

    entry1 += 3;
   
    string ret1 = SplitIndexKWhitebox::tryMatchPart(indexk1, "ccami", entry1 + 1, 2, 1);
    REQUIRE(ret1.empty());

    entry1 += 3;

    string ret2 = SplitIndexKWhitebox::tryMatchPart(indexk1, "pscci", entry1 + 1, 3, 0);
    REQUIRE(ret2.empty());
}

TEST_CASE("is trying match part correct matches for k = 1", "[split_index_k]")
{
    SplitIndexK<1> indexk1({ "index" }, hashType, 1.0f);

    // Word = psami
    char *entry1 = SplitIndexKWhitebox::createEntry<1>("ps", 2, 0);
    SplitIndexKWhitebox::addToEntry(indexk1, &entry1, "ami", 3, 1);

    entry1 += 3;
   
    string ret1 = SplitIndexKWhitebox::tryMatchPart(indexk1, "paami", entry1 + 1, 2, 1);
    REQUIRE(ret1 == string("psami"));

    entry1 += 3;

    string ret2 = SplitIndexKWhitebox::tryMatchPart(indexk1, "psamk", entry1 + 1, 3, 0);
    REQUIRE(ret2 == string("psami"));
}

TEST_CASE("is trying match part correct for k = 2", "[split_index_k]")
{
    // TODO
}

TEST_CASE("is trying match part correct for k = 3", "[split_index_k]")
{
    // TODO
}

TEST_CASE("is setting part bits correct", "[split_index_k]")
{
    const size_t nBytes = 4;
    char *entry = new char[nBytes];

    memset(entry, 0x0u, nBytes);
    *reinterpret_cast<uint16_t *>(entry) = 0x2u;

    SplitIndexKWhitebox::setPartBits<3>(entry, 0, 0);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    REQUIRE(entry[2] == 0x0u);
    REQUIRE(entry[3] == 0x0u);

    SplitIndexKWhitebox::setPartBits<3>(entry, 0, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    REQUIRE(entry[2] == 0x1u);
    REQUIRE(entry[3] == 0x0u);

    SplitIndexKWhitebox::setPartBits<3>(entry, 1, 1);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    REQUIRE(entry[2] == 0b00000101);
    REQUIRE(entry[3] == 0x0u);

    SplitIndexKWhitebox::setPartBits<3>(entry, 2, 2);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    REQUIRE(entry[2] == 0b00100101);
    REQUIRE(entry[3] == 0x0u);

    SplitIndexKWhitebox::setPartBits<3>(entry, 5, 3);

    REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    REQUIRE(entry[2] == 0b00100101);
    REQUIRE(entry[3] == 0b00001100);

    delete[] entry;
}

TEST_CASE("is retrieving part index from bits correct", "[split_index_k]")
{
    const size_t nBytes = 4;
    char *entry = new char[nBytes];

    memset(entry, 0x0u, nBytes);
    *reinterpret_cast<uint16_t *>(entry) = 0x2u;

    for (size_t iWord = 0; iWord < 8; ++iWord)
    {
        REQUIRE(SplitIndexKWhitebox::retrievePartIndexFromBits<3>(entry, iWord) == 0);
        REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    }

    entry[2] = 0x1u;
    REQUIRE(SplitIndexKWhitebox::retrievePartIndexFromBits<3>(entry, 0) == 1);
    REQUIRE(SplitIndexKWhitebox::retrievePartIndexFromBits<3>(entry, 1) == 0);

    entry[2] = 0b00100101;
    entry[3] = 0b10001100;

    vector<size_t> expected{ 1, 1, 2, 0, 0, 3, 0, 2 };

    for (size_t iWord = 0; iWord < 8; ++iWord)
    {
        REQUIRE(SplitIndexKWhitebox::retrievePartIndexFromBits<3>(entry, iWord) == expected[iWord]);
        REQUIRE(*reinterpret_cast<uint16_t *>(entry) == 0x2u);
    }

    delete[] entry;
}

} // namespace split_index
