#ifndef __FLOV_HEADER__
#define __FLOV_HEADER__

#include <type_traits>

// Container - underlying container type (std::vector<int> for example). Should
// be compatible with the following interface: class Container {
//   using
//   <any-type> push_back(const T&);
//   <any-type> push_back(T&&);
// };

// template <typename Container>
// class ContainerAdaptor {
// public:
//   using ValueType = int;
// };
template <typename Container>
class ContainerAdaptor {};

namespace detail {

namespace type_traits {

template <typename T>
struct IsNonVoid : std::true_type {};

template <>
struct IsNonVoid<void> : std::false_type {};

template <typename T>
using IfDefined = std::enable_if_t<IsNonVoid<T>::value>;

template <typename T>
struct AlwaysFalseTrait : std::false_type {};

template <typename Container, typename T = void>
struct ValueTypeImpl {
  static_assert(AlwaysFalseTrait<T>::value,
                "Container type has to has typedef'ed |::value_type| type or "
                "ContainerAdaptor<Container> defined!");
};

// std:: containers tend to provide such typedef's. Let use them.
template <typename Container>
struct ValueTypeImpl<Container, IfDefined<typename Container::value_type>> {
  using ValueType = typename Container::value_type;
};

template <typename Container>
struct ValueTypeImpl<
    Container,
    IfDefined<typename ContainerAdaptor<Container>::ValueType>> {
  using ValueType = typename ContainerAdaptor<Container>::ValueType;
};

template <typename Container>
using ValueType = typename ValueTypeImpl<Container>::ValueType;

}  // namespace type_traits

}  // namespace detail

template <typename Container>
class Flov {
 public:
  using ValueType = detail::type_traits::ValueType<Container>;
};

#endif  //__FLOV_HEADER__
