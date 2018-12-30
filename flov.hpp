#ifndef __FLOV_HEADER__
#define __FLOV_HEADER__

#include <type_traits>

// Container - underlying container type (std::vector<int> for example).
// Should be compatible with the following interface:
// class Container {
//   <any-type> push_back(const T&);
//   <any-type> push_back(T&&);
// };

namespace adaptor {

// Usage example:
//
// class X {};
//
// template <>
// class ContainerAdaptor<X> {
//  public:
//   using ValueType = vector<char>;
// };

template <typename Container>
class ContainerAdaptor {};

}  // namespace adaptor

namespace lookup_strategy {

class CacheFriendlyBinarySearch {};
}  // namespace lookup_strategy

namespace detail {

namespace type_traits {

template <typename T>
struct AlwaysFalseTrait : std::false_type {};

template <typename T>
struct AlwaysTrueTrait : std::true_type {};

template <typename T>
using IfDefined = std::enable_if_t<AlwaysTrueTrait<T>::value>;

template <typename Container, typename T = void>
struct ValueTypeImpl {
  // static_assert(AlwaysFalseTrait<T>::value,
  //               "Container type has to has typedef'ed |::value_type| type or
  //               " "ContainerAdaptor<Container> defined!");
};

// std:: containers tend to provide such typedef's. Let use them.
template <typename Container>
struct ValueTypeImpl<Container, IfDefined<typename Container::value_type>> {
  using ValueType = typename Container::value_type;
};

template <typename Container>
struct ValueTypeImpl<
    Container,
    IfDefined<typename adaptor::ContainerAdaptor<Container>::ValueType>> {
  using ValueType = typename adaptor::ContainerAdaptor<Container>::ValueType;
};

template <typename Container>
using ValueType = typename ValueTypeImpl<Container>::ValueType;

// template <class F, class... ArgTypes>
// using InvokeResultType = std::invoke_result_t<F, ArgTypes...>;

}  // namespace type_traits

}  // namespace detail

template <typename Container>
class Flov {
 public:
  using ValueType = detail::type_traits::ValueType<Container>;

  // auto push_back(const ValueType& value)
  //     -> decltype(std::declval<Container>().push_back(value)) {
  // }
};

#endif  //__FLOV_HEADER__
