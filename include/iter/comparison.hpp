#ifndef INCLUDE_ITER_COMPARISON_HPP
#define INCLUDE_ITER_COMPARISON_HPP

#include "iter/core.hpp"

template<iter::iter I1, iter::iter I2>
constexpr bool operator==(I1&& i1, I2&& i2) {
    auto item1 = iter::no_next<I1>();
    auto item2 = iter::no_next<I2>();

    while (!!iter::detail::emplace_next(item1, i1) & !!iter::detail::emplace_next(item2, i2)) {
        if (*item1 != *item2) return false;
    }

    return !!item1 == !!item2;
}

template<iter::concepts::random_access_iter I1, iter::concepts::random_access_iter I2>
constexpr bool operator==(I1&& i1, I2&& i2) {
    auto size = iter::unsafe::size(i1);
    if (size != iter::unsafe::size(i2)) return false;

    for (std::size_t i = 0; i < size; ++i) {
        auto item1 = iter::unsafe::get(i1, i);
        auto item2 = iter::unsafe::get(i2, i);
        if (item1 != item2) return false;
    }

    return true;
}

template<iter::iter I1, iter::iter I2>
constexpr auto operator<=>(I1&& i1, I2&& i2) {
    auto item1 = iter::no_next<I1>();
    auto item2 = iter::no_next<I2>();

    while (!!iter::detail::emplace_next(item1, i1)
         & !!iter::detail::emplace_next(item2, i2)) {
        if (auto rel = *item1 <=> *item2; rel != 0) return rel;
    }

    return !!item1 <=> !!item2;
}

#endif /* INCLUDE_ITER_COMPARISON_HPP */
