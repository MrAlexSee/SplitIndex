#ifndef SPLIT_INDEX_K_WHITEBOX_HPP
#define SPLIT_INDEX_K_WHITEBOX_HPP

#ifndef SPLIT_INDEX_K_WHITEBOX
#define SPLIT_INDEX_K_WHITEBOX \
    friend struct SplitIndexKWhitebox;
#endif

#include "../src/index/split_index_k.hpp"

namespace split_index
{

struct SplitIndexKWhitebox
{
    SplitIndexKWhitebox() = delete;

    template<size_t k>
    inline static size_t getPartSize(size_t wordSize)
    {
        return SplitIndexK<k>::getPartSize(wordSize);
    }
};

} // namespace split_index

#endif // SPLIT_INDEX_K_WHITEBOX_HPP
