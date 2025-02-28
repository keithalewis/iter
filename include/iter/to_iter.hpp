#ifndef INCLUDE_ITER_TO_ITER_HPP
#define INCLUDE_ITER_TO_ITER_HPP

#include "iter/core.hpp"
#include "iter/std_fwd.hpp"

namespace iter::detail {
    template<class Underlying>
    struct [[nodiscard]] container_iter {
    protected:
        using this_t = container_iter;
        static constexpr bool owner = !std::is_lvalue_reference_v<Underlying>;

        using underyling_t = std::conditional_t<owner, Underlying, std::remove_reference_t<Underlying>*>;
        underyling_t underlying;
        std::size_t pos;

        auto& get_underlying() {
            if constexpr (owner)
                return underlying;
            else
                return *underlying;
        }
        auto& get_underlying() const {
            if constexpr (owner)
                return underlying;
            else
                return *underlying;
        }

    public:
        template<class... Ts>
        requires (owner)
        container_iter(std::in_place_t, Ts&&... ins)
            : underlying{(Ts&&)ins...}
            , pos{0}
        {}

        template<class... Ts>
        requires (!owner)
        container_iter(std::in_place_t, Underlying& under)
            : underlying{&under}
            , pos{0}
        {}

        container_iter(container_iter&& other)
            : underlying{std::move(other.underlying)}
            , pos{other.pos}
        {}

        container_iter(const container_iter& other)
            : underlying{other.underlying}
            , pos{other.pos}
        {}

        container_iter& operator=(container_iter&& other) {
            underlying = std::move(other.underlying);
            pos = other.pos;
            return *this;
        }

        container_iter& operator=(const container_iter& other) {
            underlying = other.underlying;
            pos = other.pos;
            return *this;
        }

        constexpr auto ITER_UNSAFE_GET (this_t& self, std::size_t index) -> auto& {
            return self.get_underlying()[index];
        }

        constexpr auto ITER_UNSAFE_SIZE (this_t const& self) {
            return std::size(self.get_underlying());
        }

        constexpr auto ITER_IMPL_THIS(next) (this_t& self) {
            return self.pos != std::size(self.get_underlying())
                ? std::addressof(self.get_underlying()[self.pos++])
                : nullptr;
        }

        struct cycle;

        constexpr auto ITER_IMPL_THIS(cycle) (this_t&& self) requires (owner) {
            return cycle{(this_t&&) self};
        }
    };

    template<class T>
    struct container_iter<T>::cycle : container_iter<T> {
        using this_t = cycle;

        constexpr auto ITER_UNSAFE_GET (this_t& self, std::size_t index) -> auto& {
            auto size = std::size(self.get_underlying());
            return self.get_underlying()[index % size];
        }

        constexpr auto ITER_UNSAFE_SIZE (this_t const&) {
            return std::numeric_limits<std::size_t>::max();
        }

        constexpr auto ITER_IMPL_THIS(next) (this_t& self) {
            if (self.pos == std::size(self.get_underlying()))
                self.pos = 0;
            auto result = std::addressof(self.get_underlying()[self.pos]);
            ++self.pos;
            return result;
        }
     };
}

namespace iter::concepts {
    template<class T>
    static constexpr bool is_array = false;

    template<class T, std::size_t N>
    constexpr bool is_array<std::array<T, N>> = true;

    template<class T>
    concept array = is_array<std::remove_cvref_t<T>>;

    template<class T>
    static constexpr bool is_vector = false;

    template<class T, class A>
    constexpr bool is_vector<std::vector<T, A>> = true;

    template<class T>
    concept vector = is_vector<std::remove_cvref_t<T>>;

    template<class T>
    concept container = array<T> || vector<T>;
}

template<iter::concepts::container T>
constexpr auto ITER_IMPL(to_iter) (T&& container) {
    return iter::detail::container_iter<T>{std::in_place, (T&&)container};
}
template<iter::iterable T>
requires iter::concepts::container<T> && (std::remove_cvref_t<T>::owner)
constexpr auto ITER_IMPL(cycle) (T&& container) {
    return typename iter::detail::container_iter<T>::cycle{{std::in_place, (T&&)container}};
}

namespace iter::detail {
    template<class T>
    struct optional_to_iter {
        using this_t = optional_to_iter;
        std::optional<T> option;
        constexpr std::size_t ITER_UNSAFE_SIZE (this_t const& self) {
            return self.option ? 1 : 0;
        }
        constexpr decltype(auto) ITER_UNSAFE_GET (this_t& self, std::size_t) {
            return *self.option;
        }
        constexpr auto ITER_IMPL_THIS(next) (this_t& self) {
            auto r = std::move(self.option);
            self.option.reset();
            return r;
        }
    };

    template<class T>
    optional_to_iter(std::optional<T>) -> optional_to_iter<T>;
}

template<iter::concepts::optional T>
constexpr auto ITER_IMPL(to_iter) (T&& optional) {
    return iter::detail::optional_to_iter{(T&&) optional};
}

namespace iter {
    template<class T>
    struct pointer_to_iter {
        using this_t = pointer_to_iter;
        T* ptr;

        constexpr std::size_t ITER_UNSAFE_SIZE (this_t const& self) {
            return self.ptr ? 1 : 0;
        }
        constexpr decltype(auto) ITER_UNSAFE_GET (this_t& self, std::size_t) {
            return *self.ptr;
        }
        constexpr auto ITER_IMPL_THIS(next) (this_t& self) {
            auto r = self.ptr;
            self.ptr = nullptr;
            return r;
        }
    };

    template<class T>
    pointer_to_iter(T*) -> pointer_to_iter<T>;
}

#endif /* INCLUDE_ITER_TO_ITER_HPP */
