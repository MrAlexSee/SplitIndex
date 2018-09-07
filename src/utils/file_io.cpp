#include <fstream>
#include <stdexcept>

#include "file_io.hpp"

using namespace std;

namespace split_index
{

namespace utils
{

vector<string> FileIO::readFile(const string &filePath)
{
    ifstream stream(filePath);

    if (!stream)
        throw runtime_error("failed to read the file: " + filePath);

    vector<string> lines;
    string line;

    while (getline(stream, line))
    {
        lines.push_back(line); // Newlines are discarded.
    }

    return lines;
}

void FileIO::writeFile(const string &data, const string &filePath)
{
    ofstream stream(filePath);

    if (!stream)
        throw runtime_error("failed to open the file: " + filePath);

    stream << data;
}

} // namespace utils

} // namespace split_index
