#ifndef SPLIT_INDEX_K_HPP
#define SPLIT_INDEX_K_HPP

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <unordered_map>

#include "../hash_map/hash_map_aligned.hpp"
#include "../utils/bit_utils.hpp"
#include "../utils/dist.hpp"
#include "split_index.hpp"

template<int k>
class SplitIndexK : public SplitIndex
{
public:
    SplitIndexK(const std::vector<std::string> &words, int minWordLength = -1);
    ~SplitIndexK();

    std::string toString() const override;

    void construct() override;

    std::string prettyResults(const std::string &results) const override;
    void addPartResult(const std::string &parts, const std::string &query,
                       std::string &results) const;
protected:
    void initEntry(const std::string &word) override;
    void splitWord(const std::string &word) override;

    virtual char *createEntry(const std::string &wordPart, int iPart) const;
    virtual void addToEntry(char **entryPtr, const std::string &wordPart, int iPart) const;
    virtual void appendToEntry(char *newEntry, const std::string &wordPart, int startNew) const;

    static void addNewMarker(char *entry, size_t oldSize);

    // iPart = 0 -> 0,0
    // iPart = 1 -> 0,1
    // iPart = 2 -> 1,0
    // iPart = 3 -> 1,1
    static void setPartBit(char *entry, size_t iWord, size_t iPart);
    static int getPartBit(const char *entry, size_t iWord);

    void processQuery(const std::string &query, std::string &results) override;
    bool processQueryPart(const std::string &query, int iPart, std::string &results) const;

    bool tryMatchPart(const std::string &query, const char *entry,
                      char matchSize, int iPart, std::string &results) const;

    bool tryMatchParts(const std::string &query, const char *entry,
                       int iPart, std::string &results) const;

    size_t calcEntrySize(const char *entry) const override;
    virtual int calcEntryNWords(const char *entry) const;

    size_t getPartSize(size_t wordSize) const override
    {
        return std::floor(static_cast<double>(wordSize) / (k + 1));
    }

    std::string *parts = nullptr;

    // There 2 bits (4 positions) per word.
    static constexpr const int maxK = 3;
};

template<int k>
SplitIndexK<k>::SplitIndexK(const std::vector<std::string> &words, int minWordLength)
    :SplitIndex(words, minWordLength)
{
    int sizeHint = wordSet.size() * sizeHintFactor;

    auto fun = std::bind(&SplitIndexK<k>::calcEntrySize, this, std::placeholders::_1);
    map = new HashMapAligned(fun, maxLoadFactor, sizeHint);

    assert(k >= 1 and k <= maxK);
    parts = new std::string[k + 1];

    for (int i = 0; i <= k; ++i)
    {
        parts[i].reserve(maxWordSize);
    }

    std::cout << boost::format("Initialized %1% part buffers (k = %2%)")
                 % (k + 1) % k << std::endl;
}

template<int k>
SplitIndexK<k>::~SplitIndexK()
{
    delete[] parts;
}

template<int k>
std::string SplitIndexK<k>::toString() const
{
    return SplitIndex::toString();
}

template<int k>
void SplitIndexK<k>::construct()
{
    SplitIndex::construct();
}

template<int k>
std::string SplitIndexK<k>::prettyResults(const std::string &results) const
{
    std::string finalRes = "";

    std::vector<std::string> lines;
    boost::split(lines, results, boost::is_any_of("\n"));

    std::vector<std::string> parts;

    for (const std::string &line : lines)
    {
        if (line == "" or line == "\n")
            continue;

        boost::split(parts, line, boost::is_any_of(" "));
        std::string query = parts.back();

        finalRes += query + ": [";

        for (int i = 0; i < parts.size() - 1; ++i)
        {
            finalRes += "\"";
            addPartResult(parts[i], query, finalRes);
            finalRes += "\"";

            if (i != parts.size() - 2)
            {
                finalRes += ", ";
            }
        }

        finalRes += "]\n";
    }

    return finalRes;
}

template<int k>
void SplitIndexK<k>::addPartResult(const std::string &part, const std::string &query,
                                   std::string &results) const
{
    assert(part.back() - '0' <= k);
    
    size_t partSize = getPartSize(query.size());
    std::string lastPart;

    switch (part.back())
    {
        case '0':
            results += query.substr(0, partSize);
            results += part.substr(0, part.size() - 1);
            break;
        case '1':
            switch (k)
            {
                case 1:
                    results += part.substr(0, part.size() - 1);
                    results += query.substr(partSize, std::string::npos);
                    break;
                case 2: case 3:
                    results += part.substr(0, partSize);
                    results += query.substr(partSize, partSize);
                    
                    lastPart = part.substr(partSize, std::string::npos);
                    results += lastPart.substr(0, lastPart.size() - 1);
                    break;
                default:
                    assert(false);
            }
            break;
        case '2':
            assert(k >= 2);

            switch (k)
            {
                case 2: // k == 2
                    results += part.substr(0, part.size() - 1);
                    results += query.substr(2 * partSize, std::string::npos);
                    break;
                case 3: // k == 3
                    results += part.substr(0, 2 * partSize);
                    results += query.substr(2 * partSize, partSize);

                    lastPart = part.substr(2 * partSize, std::string::npos);
                    results += lastPart.substr(0, lastPart.size() - 1);
                    break;
                default:
                    assert(false);
            }
            break;
        case '3':
            assert(k == 3);

            results += part.substr(0, part.size() - 1);
            results += query.substr(3 * partSize, std::string::npos);
            break;
        default:
            assert(false);
    }        
}

template<int k>
void SplitIndexK<k>::initEntry(const std::string &word)
{    
    splitWord(word);

    size_t partSize = getPartSize(word.size());
    assert(partSize >= 1);

    for (int i = 0; i <= k; ++i)
    {
        assert(parts[i].size() > 0);
        size_t start = i * partSize;

        std::string wordPart = word;

        if (i != k)
        {
            wordPart.erase(start, partSize);
        }
        else
        {
            wordPart.erase(start, std::string::npos);
        }
        assert(word.size() == wordPart.size() + parts[i].size());

        char **entryPtr = map->get(parts[i].c_str(), parts[i].size());

        if (entryPtr == nullptr)
        {
            char *entry = createEntry(wordPart, i);
            map->insert(parts[i].c_str(), parts[i].size(), entry);

            free(entry); // Copied inside the map.
        }
        else
        {
            addToEntry(entryPtr, wordPart, i);
        }
    }
}

template<int k>
void SplitIndexK<k>::splitWord(const std::string &word)
{
    size_t partSize = getPartSize(word.size());
    assert(partSize >= 1);
        
    for (int i = 0; i <= k; ++i)
    {
        size_t start = i * partSize;

        if (i != k)
        {
            parts[i] = word.substr(start, partSize);
            assert(parts[i].size() == partSize); 
        }
        else
        {
            parts[i] = word.substr(start, std::string::npos);
            assert(parts[i].size() >= 1);
        }
    }
}

template<int k>
char *SplitIndexK<k>::createEntry(const std::string &wordPart, int iPart) const
{
    assert(wordPart.size() > 0 and wordPart.size() <= maxWordSize);
    size_t newSize = 4 + wordPart.size() + 1; 

    char *entry = static_cast<char *>(malloc(newSize));
    assert(entry != nullptr);

    // Index byte counter
    *reinterpret_cast<uint16_t *>(entry) = 0x1;
    entry[2] = 0x0; // Originally no bits are set

    if (iPart != 0)
    {
        setPartBit(entry, 0, iPart);
    }

    entry[3] = static_cast<char>(wordPart.size());

    for (size_t i = 0; i < wordPart.size(); ++i)
    {
        entry[4 + i] = wordPart[i];
    }
    
    entry[newSize - 1] = '\0';

    assert(4 + wordPart.size() == newSize - 1);
    return entry;
}

template<int k>
void SplitIndexK<k>::addToEntry(char **entryPtr, const std::string &wordPart, int iPart) const
{
    assert(wordPart.size() > 0 and wordPart.size() <= maxWordSize);
    uint16_t nMarkers = *reinterpret_cast<const uint16_t *>(*entryPtr);
    
    size_t oldSize = calcEntrySize(*entryPtr);
    int oldNWords = calcEntryNWords(*entryPtr);

    assert(oldNWords <= nMarkers * 4);

    size_t newSize = oldSize + 1 + wordPart.size();
    bool newMarker = false;

    if (oldNWords == nMarkers * 4)
    {
        // All markers are currently in use so we have to add a new one.
        newMarker = true;
        newSize += 1;
    }

    char *newEntry = static_cast<char *>(realloc(*entryPtr, newSize));
    assert(newEntry != nullptr);

    if (newMarker)
    {
        addNewMarker(newEntry, oldSize);
        
        appendToEntry(newEntry, wordPart, oldSize);
        assert(oldSize + 1 + wordPart.size() == newSize - 1);
    }
    else
    {
        // We will start overwriting with the terminating '\0'.
        appendToEntry(newEntry, wordPart, oldSize - 1);
        assert(oldSize + wordPart.size() == newSize - 1);
    }
    assert(calcEntryNWords(newEntry) == oldNWords + 1);

    if (iPart != 0) // No need to set any bits for 0.
    {
        // 0-indexed, hence we use the old #words value.
        setPartBit(newEntry, oldNWords, iPart); 
    }

    // Required in the case the memory has been moved; no need for free (realloc does that).
    *entryPtr = newEntry;
}

template<int k>
void SplitIndexK<k>::appendToEntry(char *newEntry, const std::string &wordPart, int startNew) const
{
    newEntry[startNew] = static_cast<char>(wordPart.size());
    
    for (size_t i = 0; i < wordPart.size(); ++i)
    {
        newEntry[startNew + 1 + i] = wordPart[i];
    }

    newEntry[startNew + 1 + wordPart.size()] = '\0';
}

template<int k>
void SplitIndexK<k>::addNewMarker(char *entry, size_t oldSize)
{
    uint16_t *nMarkers = reinterpret_cast<uint16_t *>(entry);
    size_t wordsStart = sizeof(uint16_t) + *nMarkers;

    moveToRight(entry + wordsStart, 1, oldSize - wordsStart);
    *nMarkers += 1;

    // The new marker is where the words used to begin.
    entry[wordsStart] = 0x0;
}

template<int k>
void SplitIndexK<k>::setPartBit(char *entry, size_t iWord, size_t iPart)
{
    assert(iPart >= 0 and iPart <= k); // 0-indexed, #parts == k + 1
    size_t pos = iWord * 2;

    size_t byte = pos / 8;
    size_t bit = pos % 8;

    entry += sizeof(uint16_t); // Go to the first byte
    entry += byte;

    switch (iPart)
    {
        case 0:
            break;
        case 1:
            // Won't overflow because we have multiples of 2 mod 8.
            bit += 1;
            BitUtils::setBit(entry, bit);
            break;
        case 2:
            assert(k >= 2);
            BitUtils::setBit(entry, bit);
            break;
        case 3:
            assert(k == 3);
            BitUtils::setBit(entry, bit);
       
            bit += 1;
            BitUtils::setBit(entry, bit);
            break;
        default:
            assert(false);
    }

    assert(getPartBit(entry - sizeof(uint16_t) - byte, iWord) == iPart);
}

template<int k>
int SplitIndexK<k>::getPartBit(const char *entry, size_t iWord)
{
    size_t pos = iWord * 2;

    // There are 4 words per byte.
    size_t byte = pos / 8; 
    size_t bit = pos % 8;

    entry += sizeof(uint16_t); // Go to the first byte
    entry += byte;

    bool first = BitUtils::isBitSet(*entry, bit);

    bit += 1; // Won't overflow because we have multiples of 2 mod 8.
    bool second = BitUtils::isBitSet(*entry, bit);

    if (first)
    {
        if (second)
            return 3;
        else
            return 2;
    }
    else
    {
        if (second)
            return 1;
        else
            return 0;
    }

    assert(false);
}

template<int k>
void SplitIndexK<k>::processQuery(const std::string &query, std::string &results)
{
    assert(constructed);

    if (query.size() <= k or query.size() > maxWordSize)
    {
    #ifndef NO_ERROR_MSG
        std::cout << boost::format("Ignoring invalid query: %1%, size = %2%")
                                   % query % query.size() << std::endl;
    #endif
        return;
    }

    splitWord(query);
    bool hasResults = false;

    for (int i = 0; i <= k; ++i)
    {
        if (processQueryPart(query, i, results))
        {
            hasResults = true;
        }
    }

    if (hasResults)
    {
        results.append(query);
        results.append(1, '\n');
    }
}

template<int k>
bool SplitIndexK<k>::processQueryPart(const std::string &query, int iPart, std::string &results) const
{
    char **entryPtr = map->get(parts[iPart].c_str(), parts[iPart].size());

    if (entryPtr == nullptr)
        return false;

    char cMatchSize = query.size() - parts[iPart].size();
    size_t iWord = 0;
    bool hasResults = false;

    const char *entry = *entryPtr;
    entry += sizeof(uint16_t) + (*(reinterpret_cast<const uint16_t *>(entry)));

    while (entry[0] != 0)
    {
        if (entry[0] == cMatchSize)
        {
            int index = getPartBit(*entryPtr, iWord);

            if (index == iPart)
            {
                if (tryMatchPart(query, entry + 1,
                                 cMatchSize, index, results))
                {
                    hasResults = true;
                }
            }
        }

        iWord += 1;
        entry += 1 + entry[0];
    }

    return hasResults;
}

template<int k>
bool SplitIndexK<k>::tryMatchPart(const std::string &query, const char *entry,
                                  char matchSize, int iPart, std::string &results) const
{
    bool hasResults = false;

    switch (iPart)
    {
        case 0:
            if (Dist::isHammingK<k>(entry, query.c_str() + parts[iPart].size(), matchSize))
            {
                addResult(entry, matchSize, results, '0');
                hasResults = true;
            }
            break;
        case 1:
            switch (k)
            {
                case 1: // k == 1
                    if (Dist::isHammingK<k>(entry, query.c_str(), matchSize))
                    {
                        addResult(entry, matchSize, results, '1');
                        hasResults = true;
                    }
                    break;
                case 2: case 3: // k == 2/3
                    if (tryMatchParts(query, entry, 1, results))
                    {
                        hasResults = true;
                    }
                    break;
                default:
                    assert(false);
            }

            break;
        case 2:
            assert(k >= 2);

            switch (k)
            {
                case 2: // k == 2
                    if (Dist::isHammingK<k>(entry, query.c_str(), matchSize))
                    {
                        addResult(entry, matchSize, results, '2');
                        hasResults = true;
                    }
                    break;
                case 3: // k == 3
                    if (tryMatchParts(query, entry, 2, results))
                    {
                        hasResults = true;
                    }
                    break;
                default:
                    assert(false);
            }

            break;

        case 3:
            assert(k == 3);

            if (Dist::isHammingK<3>(entry, query.c_str(), matchSize))
            {
                addResult(entry, matchSize, results, '3');
                hasResults = true;
            }

            break;
        default:
            assert(false);
    }

    return hasResults;
}

template<int k>
bool SplitIndexK<k>::tryMatchParts(const std::string &query, const char *entry,
                                   int iPart, std::string &results) const
{
    assert(k == 2 or k == 3);
    size_t partSize1 = 0, partSize2 = 0;

    switch (k)
    {
        case 2:
            assert(iPart == 1);
            partSize1 = parts[0].size();
            partSize2 = parts[2].size();
            break;
        case 3:
            assert(iPart == 1 or iPart == 2);
            switch (iPart)
            {
                case 1:
                    partSize1 = parts[0].size();
                    partSize2 = parts[2].size() + parts[3].size();
                    break;
                case 2:
                    partSize1 = parts[0].size() + parts[1].size();
                    partSize2 = parts[3].size();
                    break;
                default:
                    assert(false);
            }
            break;
        default:
            assert(false);
    }

    assert(partSize1 + parts[iPart].size() + partSize2 == query.size());
    
    int nErrors = Dist::calcHamming(entry, query.c_str(), partSize1);
    nErrors += Dist::calcHamming(entry + partSize1, query.c_str() + query.size() - partSize2, partSize2);

    if (nErrors <= k)
    {
        std::string result = std::string(entry, partSize1) + std::string(entry + partSize1, partSize2);
        char partChar = '0' + static_cast<char>(iPart);

        addResult(entry, partSize1 + partSize2, results, partChar);
        return true;
    }

    return false;
}

template<int k>
size_t SplitIndexK<k>::calcEntrySize(const char *entry) const
{
    const char *start = entry;
    entry += sizeof(uint16_t) + *reinterpret_cast<const uint16_t *>(entry);

    while (entry[0] != 0)
    {
        entry += 1 + entry[0];
    }

    return entry - start + 1; // Including the terminating '\0'.
}

template<int k>
int SplitIndexK<k>::calcEntryNWords(const char *entry) const
{
    int nWords = 0;
    entry += sizeof(uint16_t) + *reinterpret_cast<const uint16_t *>(entry);

    while (entry[0] != 0)
    {
        nWords += 1;
        entry += 1 + entry[0];
    }

    return nWords;
}

#endif // SPLIT_INDEX_K_HPP
