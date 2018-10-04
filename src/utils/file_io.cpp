#include <boost/algorithm/string.hpp>
#include <fstream>
#include <stdexcept>

#include "file_io.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

bool FileIO::isFileReadable(const string &filePath)
{
    ifstream inStream(filePath);
    return inStream.good();
}

vector<string> FileIO::readWords(const string &filePath, const string &separator)
{
    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    const string text = static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();

    vector<string> words;
    boost::split(words, text, boost::is_any_of(separator));

    vector<string> filt;

    for (string &word : words)
    {
        boost::trim(word);

        if (word.empty() == false) 
        {
            filt.emplace_back(move(word));
        }
    }

    return filt;
}

void FileIO::writeFile(const string &data, const string &filePath)
{
    ofstream outStream(filePath);

    if (!outStream)
    {
        throw std::runtime_error("failed to write file (insufficient permisions?): " + filePath);
    }

    outStream << data;
}

} // namespace utils

} // namespace split_index
