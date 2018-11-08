#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include <string>
#include <vector>

namespace split_index
{

namespace utils
{

struct FileIO
{
    FileIO() = delete;

    static bool isFileReadable(const std::string &filePath);
    static bool isFileEmpty(const std::string &filePath);

    static std::vector<std::string> readWords(const std::string &filePath, const std::string &separator);

    /** Appends [text] to file with [filePath] followed by an optional newline if [newline] is true. */
    static void dumpToFile(const std::string &text, const std::string &filePath, bool newline = false);
};

} // namespace utils

} // namespace split_index

#endif // FILE_IO_HPP
