#ifndef INCLUDE_ITER_FILTER_MAP_HPP
#define INCLUDE_ITER_FILTER_MAP_HPP

#include "iter/core.hpp"

ITER_DECLARE(filter_map)

namespace iter::detail {
    template<iter I, std::invocable<consume_t<I>> F>
    struct [[nodiscard]] filter_map_iter {
        using this_t = filter_map_iter;
        using mapped_t = std::invoke_result_t<F, ref_t<I>>;
        static_assert(concepts::optional_next<mapped_t> || concepts::pointer_next<mapped_t>);

        I i;
        [[no_unique_address]] F func;

        constexpr mapped_t ITER_IMPL_THIS(next) (this_t& self) {
            auto mapped = mapped_t{};
            ITER_FOR (val, self.i) {
                if (EMPLACE_NEW(mapped, self.func(consume(val)))) {
                    return mapped;
                }
            }
            return mapped;
        }
    };

    template<class I, class P>
    filter_map_iter(I, P) -> filter_map_iter<I, P>;
}

template<iter::iterable I, std::invocable<iter::consume_t<I>> F>
constexpr auto ITER_IMPL(filter_map) (I&& iterable, F&& func) {
    return iter::detail::filter_map_iter{iter::to_iter((I&&) iterable), (F&&) func};
}

#endif /* INCLUDE_ITER_FILTER_MAP_HPP */
