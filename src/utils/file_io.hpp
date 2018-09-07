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

    static std::vector<std::string> readFile(const std::string &filePath);
    static void writeFile(const std::string &data, const std::string &filePath);
};

} // namespace utils

} // namespace split_index

#endif // FILE_IO_HPP
