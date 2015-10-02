#ifndef TEMPLATEFUNCTIONS_H
#define TEMPLATEFUNCTIONS_H

#include <type_traits>

namespace detail {
    enum class enabler {};
}

template <bool Condition>
using EnableIf = typename std::enable_if<Condition, detail::enabler>::type;

template<typename... Conds>
struct and_ : std::true_type {};

template<typename Cond, typename... Conds>
struct and_<Cond, Conds...>
        : std::conditional<Cond::value, and_<Conds...>,
        std::false_type>::type {};

template<typename Target, typename... Ts>
using areT = and_<std::is_same<Ts, Target>...>;

template<size_t S1, size_t S2>
struct max
{
    const static size_t value = S1 > S2 ? S1 : S2;
};

#endif // TEMPLATEFUNCTIONS_H

