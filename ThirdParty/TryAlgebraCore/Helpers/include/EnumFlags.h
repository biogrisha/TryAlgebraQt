#pragma once
#include <type_traits>

// Disabled by default
template<typename E>
struct enable_bitmask_operators : std::false_type {};

// operator|
template<typename E>
constexpr std::enable_if_t<enable_bitmask_operators<E>::value, E>
operator|(E lhs, E rhs)
{
    using U = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<U>(lhs) | static_cast<U>(rhs));
}

// operator&
template<typename E>
constexpr std::enable_if_t<enable_bitmask_operators<E>::value, E>
operator&(E lhs, E rhs)
{
    using U = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<U>(lhs) & static_cast<U>(rhs));
}

// operator|=
template<typename E>
constexpr std::enable_if_t<enable_bitmask_operators<E>::value, E&>
operator|=(E& lhs, E rhs)
{
    return lhs = lhs | rhs;
}

template<typename E>
constexpr bool hasFlag(E value, E flag)
{
    return (value & flag) != static_cast<E>(0);
}