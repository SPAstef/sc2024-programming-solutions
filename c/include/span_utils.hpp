#pragma once

#include <ranges>
#include <span>

template<std::ranges::range Range, typename T, size_t E>
Range range_from_span(std::span<T, E> s)
{
    return []<size_t... i>(std::span<T, E> s, std::index_sequence<i...>)
    { return Range{s[i]...}; }(s, std::make_index_sequence<E>{});
}

template<typename T, typename F, size_t sz_f>
auto span_cast(std::span<F, sz_f> s)
{
    static constexpr size_t sz_t = (sz_f * sizeof(F)) / sizeof(T);

    return std::span<T, sz_t>{reinterpret_cast<T *>(s.data()), sz_t};
}
