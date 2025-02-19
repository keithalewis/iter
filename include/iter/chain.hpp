#ifndef INCLUDE_ITER_CHAIN_HPP
#define INCLUDE_ITER_CHAIN_HPP

#include "iter/core.hpp"

ITER_DECLARE(chain)

namespace iter::detail {
    template<iter I1, iter I2>
    struct [[nodiscard]] chain_iter : enable_random_access<chain_iter<I1, I2>, I1, I2> {
        static_assert(std::same_as<value_t<I1>, value_t<I2>>);

        template<class T, class U>
        constexpr chain_iter(T&& i1, U&& i2) : i1{(T&&)i1}, i2{(U&&)i2}
        {
            if constexpr (this_t::random_access) {
                this->size = iter::unsafe::size(*this->i1) + iter::unsafe::size(this->i2);
            }
        }

    private:
        using this_t = chain_iter;

        std::optional<I1> i1;
        I2 i2;

        constexpr decltype(auto) ITER_UNSAFE_GET (this_t& self, std::size_t index)
            requires this_t::random_access
        {
            std::size_t i1s = iter::unsafe::size(*self.i1);
            return index < i1s ? iter::unsafe::get(*self.i1, index) : iter::unsafe::get(self.i2, index - i1s);
        }

        static constexpr bool optional_next = concepts::optional_iter<I1> || concepts::optional_iter<I2>;
        using next_t = std::conditional_t<optional_next, std::optional<value_t<I1>>, value_t<I1>*>;

        constexpr next_t ITER_IMPL_THIS(next) (this_t& self)
            requires (!this_t::random_access)
        {
            auto val = next_t{};
            if (self.i1) {
                if constexpr (optional_next && !concepts::optional_iter<I1>) {
                    if (auto pval = iter::next(*self.i1)) {
                        val.emplace(*pval);
                        return val;
                    }
                } else {
                    if (emplace_next(val, *self.i1))
                        return val;
                }
                // If we haven't returned by this point, we reached the end of I1
                self.i1.reset();
            }
            if constexpr (optional_next && !concepts::optional_iter<I2>) {
                if (auto pval = iter::next(self.i2))
                    val.emplace(*pval);
            } else {
                emplace_next(val, self.i2);
            }
            return val;
        }
    };

    template<class I1, class I2>
    chain_iter(I1, I2) -> chain_iter<I1, I2>;
}

template<iter::iterable I1, iter::iterable I2>
constexpr auto ITER_IMPL(chain) (I1&& iterable1, I2&& iterable2) {
    return iter::detail::chain_iter{iter::to_iter((I1&&) iterable1),
                                    iter::to_iter((I2&&) iterable2)};
}

#endif /* INCLUDE_ITER_CHAIN_HPP */
