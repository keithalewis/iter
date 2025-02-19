#ifndef INCLUDE_ITER_NTH_HPP
#define INCLUDE_ITER_NTH_HPP

#include "iter/core.hpp"

ITER_DECLARE(nth)

template<iter::concepts::random_access_iterable I>
constexpr auto ITER_IMPL(nth) (I&& iterable, std::size_t n) {
    decltype(auto) iter = iter::to_iter((I&&) iterable);
    std::size_t size = iter::unsafe::size(iter);
    using get_t = decltype(iter::unsafe::get(iter, n));
    if constexpr (std::is_lvalue_reference_v<I> && std::is_lvalue_reference_v<get_t>)
        return size > n ? std::addressof(iter::unsafe::get(iter, n)) : nullptr;
    else
        return size > n ? MAKE_OPTIONAL(iter::unsafe::get(iter, n)) : std::nullopt;
}

template<iter::concepts::random_access_iterable I, class T>
constexpr auto ITER_IMPL(nth) (I&& iterable, std::size_t n, T&& fallback) {
    decltype(auto) iter = iter::to_iter((I&&) iterable);
    std::size_t size = iter::unsafe::size(iter);
    return size > n ? iter::unsafe::get(iter, n) : (T&&)fallback;
}

template<iter::iterable I>
constexpr auto ITER_IMPL(nth) (I&& iterable, std::size_t n) {
    decltype(auto) iter = iter::to_iter((I&&) iterable);
    auto result = iter::no_next<decltype(iter)>();
    while (iter::detail::emplace_next(result, iter) && n-- > 0);
    if constexpr (iter::concepts::optional_iterable<I> || std::is_lvalue_reference_v<I>)
        return result;
    else
        return result ? std::make_optional(*result) : std::nullopt;
}

template<iter::iterable I, class T>
constexpr auto ITER_IMPL(nth) (I&& iterable, std::size_t n, T&& fallback) {
    decltype(auto) iter = iter::to_iter((I&&) iterable);
    auto result = iter::no_next<decltype(iter)>();
    while (iter::detail::emplace_next(result, iter) && n-- > 0);
    if constexpr (iter::concepts::optional_iterable<I>)
        return result ? std::move(*result) : (T&&) fallback;
    else
        return result ? *result : (T&&) fallback;
}
#endif /* INCLUDE_ITER_NTH_HPP */
