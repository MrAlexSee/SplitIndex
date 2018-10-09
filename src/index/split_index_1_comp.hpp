#ifndef SPLIT_INDEX_1_COMP_HPP
#define SPLIT_INDEX_1_COMP_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "split_index_1.hpp"

namespace split_index
{

class SplitIndex1Comp : public SplitIndex1
{
public:
    SplitIndex1Comp(const std::vector<std::string> &words, int minWordLength = -1);
    ~SplitIndex1Comp();

    std::string toString() const override;

    void construct() override;

protected:
    virtual void constructQgramLists();
    void constructQGrams(int nQgrams, int qgramSize, char firstChar);

    // Returns a list of all qgrams, most common qgrams first.
    std::vector<std::string> constructQGramOrder(int qgramSize);

    void initEntry(const std::string &word) override;

    // The part is a prefix.
    bool searchPartPref(const char *keyPart, size_t keySize,
                        const char *matchPart, size_t matchSize, std::string &results) override;
    // The part is a suffix.
    bool searchPartSuf(const char *keyPart, size_t keySize,
                       const char *matchPart, size_t matchSize, std::string &results) override;
  
    virtual size_t encode(const char *word, size_t wordSize);
    virtual size_t decode(const char *word, size_t wordSize, size_t maxSize);

    // Encodes the word to the buffer.
    // Returns the size of the encoded word.
    size_t encodeToBuf(const char *word, size_t wordSize, char *buf, int qgramSize);

    // Decodes the word to the buffer.
    // MaxSize allows us to finish early if the decoded word would be too long anyway for the Hamming distance.
    // Returns the size of the decoded word or 0 if maxSize is exceeded.
    size_t decodeToBuf(const char *word, size_t wordSize, char *buf, size_t maxSize);

    char *codingBuf = nullptr;

    std::unordered_map<std::string, char> qgramToChar;
    std::unordered_map<char, std::string> charToQgram;

    const int nQgrams = 100;
    const int qgramSize = 2; // E.g., 2 for digrams.
    
    // First char used for q-gram encoding (the rest is consecutive).
    // Make sure that chars are unsigned.
    const char firstChar = 128;
};

} // namespace split_index

#endif // SPLIT_INDEX_1_COMP_HPP
