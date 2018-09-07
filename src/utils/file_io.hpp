#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <string>
#include <vector>

namespace split_index
{

namespace utils
{

class FileIO
{
public:
    FileIO() = delete;

    static bool isFileReadable(const std::string &filePath);
    static std::vector<std::string> readWords(const std::string &filePath, const std::string &separator);

    static void writeFile(const std::string &data, const std::string &filePath);
};

} // namespace utils

} // namespace split_index

#endif // FILE_IO_HPP
