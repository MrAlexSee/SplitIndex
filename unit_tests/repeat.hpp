#ifndef REPEAT_HPP
#define REPEAT_HPP

#include <functional>

inline void repeat(int count, std::function<void()> fun)
{
    for (int i = 0; i < count; ++i)
    {
        fun();
    }
}

// https://stackoverflow.com/questions/37602057/why-isnt-a-for-loop-a-compile-time-expression
template<size_t N>
struct NumberBox { static const constexpr auto value = N; };

template <class FunctionType, size_t... Count>
void for_(FunctionType fun, std::index_sequence<Count...>)
{
    using expander = int[];
    (void)expander{ 0, ((void)fun(NumberBox<Count>{ }), 0)... };
}

template <size_t N, typename FunctionType>
void for_(FunctionType fun)
{
    for_(fun, std::make_index_sequence<N>());
}

#endif // REPEAT_HPP
