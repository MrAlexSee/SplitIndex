#include "catch.hpp"

#include "../src/utils/file_io.hpp"

using namespace std;

namespace split_index
{

namespace
{

const string tmpFileName = "tmp.dat";

}

inline void removeFile(const string &filePath)
{
    remove(filePath.c_str());
}

TEST_CASE("is querying empty file correct", "[utils_file_io]")
{
    REQUIRE(utils::FileIO::isFileEmpty(tmpFileName));

    utils::FileIO::dumpToFile("", tmpFileName, false);
    REQUIRE(utils::FileIO::isFileEmpty(tmpFileName));

    utils::FileIO::dumpToFile("abc", tmpFileName, false);
    REQUIRE(utils::FileIO::isFileEmpty(tmpFileName) == false);

    removeFile(tmpFileName);
}

TEST_CASE("is reading empty words correct", "[utils_file_io]")
{
    utils::FileIO::dumpToFile("", tmpFileName, false);

    vector<string> words = utils::FileIO::readWords(tmpFileName, "\n");
    REQUIRE(words.size() == 0);

    removeFile(tmpFileName);
    REQUIRE(utils::FileIO::isFileReadable(tmpFileName) == false);
}

TEST_CASE("is reading words correct", "[utils_file_io]")
{
    string str = "ala\nma\nkota";
    utils::FileIO::dumpToFile(str, tmpFileName, false);

    vector<string> words = utils::FileIO::readWords(tmpFileName, "\n");

    REQUIRE(words.size() == 3);
    REQUIRE(words == vector<string> { "ala", "ma", "kota" });

    removeFile(tmpFileName);
    REQUIRE(utils::FileIO::isFileReadable(tmpFileName) == false);
}

TEST_CASE("is reading words with semicolon separator correct", "[utils_file_io]")
{
    string str = "ala;ma;kota";
    utils::FileIO::dumpToFile(str, tmpFileName, false);

    vector<string> words = utils::FileIO::readWords(tmpFileName, ";");

    REQUIRE(words.size() == 3);
    REQUIRE(words == vector<string> { "ala", "ma", "kota" });

    removeFile(tmpFileName);
    REQUIRE(utils::FileIO::isFileReadable(tmpFileName) == false);
}

TEST_CASE("is reading words with whitespace correct", "[utils_file_io]")
{
    string str = "ala\nma  \n\n\n\n\n kota  \n";
    utils::FileIO::dumpToFile(str, tmpFileName, false);

    vector<string> words = utils::FileIO::readWords(tmpFileName, "\n");

    REQUIRE(words.size() == 3);
    REQUIRE(words == vector<string> { "ala", "ma", "kota" });

    removeFile(tmpFileName);
    REQUIRE(utils::FileIO::isFileReadable(tmpFileName) == false);
}

} // namespace split_index
