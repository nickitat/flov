#pragma once

#include "intf.hpp"

#include <type_traits>

namespace detail {

namespace type_traits {

template <typename T>
struct AlwaysFalseTrait : std::false_type {};

template <typename T>
struct AlwaysTrueTrait : std::true_type {};

template <typename Lhs, typename Rhs>
/*inline*/ constexpr bool IsSame = std::is_same<Lhs, Rhs>::value;

// should be std::void_t but clang currently has wrong behaviour in this case
// (bug seems to be fixed https://bugs.llvm.org/show_bug.cgi?id=39623)
template <typename... T>
struct VoidImpl {
  using type = void;
};
template <typename... T>
using Void = typename VoidImpl<T...>::type;

// based on https://stackoverflow.com/a/20852289
template <bool,
          template <typename...> class IfTrueType,
          template <typename...> class IfFalseType,
          typename... Ts>
struct ConditionalApplyImpl {
  using type = IfTrueType<Ts...>;
};
template <template <typename...> class IfTrueType,
          template <typename...> class IfFalseType,
          typename... Ts>
struct ConditionalApplyImpl<false, IfTrueType, IfFalseType, Ts...> {
  using type = IfFalseType<Ts...>;
};
template <bool b,
          template <typename...> class IfTrueType,
          template <typename...> class IfFalseType,
          typename... Ts>
using ConditionalApply =
    typename ConditionalApplyImpl<b, IfTrueType, IfFalseType, Ts...>::type;

template <typename Container, typename T = void>
struct ValueTypeImpl {};
// std:: containers tend to provide such typedef's. Let use them.
template <typename Container>
struct ValueTypeImpl<Container, Void<typename Container::value_type>> {
  using ValueType = typename Container::value_type;
};
template <typename Container>
struct ValueTypeImpl<
    Container,
    Void<typename adaptor::ContainerAdaptor<Container>::ValueType>> {
  using ValueType = typename adaptor::ContainerAdaptor<Container>::ValueType;
};
template <typename Container>
using ValueType = typename ValueTypeImpl<Container>::ValueType;

}  // namespace type_traits

}  // namespace detail
