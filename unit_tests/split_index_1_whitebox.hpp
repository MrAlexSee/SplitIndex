#ifndef SPLIT_INDEX_1_WHITEBOX_HPP
#define SPLIT_INDEX_1_WHITEBOX_HPP

#ifndef SPLIT_INDEX_1_WHITEBOX
#define SPLIT_INDEX_1_WHITEBOX \
    friend struct SplitIndex1Whitebox;
#endif

#include "../src/index/split_index_1.hpp"


namespace split_index
{

struct SplitIndex1Whitebox
{
    SplitIndex1Whitebox() = delete;
};

} // namespace split_index

#endif // SPLIT_INDEX_1_WHITEBOX_HPP
