#include <iostream>
#include <string>
#include <vector>
using namespace std;

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

template <typename Lhs, typename Rhs>
/*inline*/ constexpr bool IsSame = std::is_same<Lhs, Rhs>::value;
    
// should be std::void_t but clang currently has wrong behaviour in this case (bug seems to be fixed https://bugs.llvm.org/show_bug.cgi?id=39623)
template<typename ...T>
struct VoidImpl {
  using type = void;
};    
template<typename ...T>
using Void = typename VoidImpl<T...>::type;  
    
// based on https://stackoverflow.com/a/20852289
template<bool, template<typename...>class IfTrueType, template<typename...>class IfFalseType, typename... Ts>
struct ConditionalApplyImpl {
  using type = IfTrueType<Ts...>;
};
template<template<typename...>class IfTrueType, template<typename...>class IfFalseType, typename...Ts>
struct ConditionalApplyImpl<false, IfTrueType, IfFalseType, Ts...> {
  using type = IfFalseType<Ts...>;
};
template<bool b, template<typename...>class IfTrueType, template<typename...>class IfFalseType, typename... Ts>
using ConditionalApply=typename ConditionalApplyImpl<b, IfTrueType, IfFalseType, Ts...>::type;
    
template <typename Container, typename T = void>
struct ValueTypeImpl {
  static_assert(AlwaysFalseTrait<T>::value, "Container type has to has typedef'ed |::value_type| type or ContainerAdaptor<Container> defined!");
};
// std:: containers tend to provide such typedef's. Let use them.
template <typename Container>
struct ValueTypeImpl<Container, Void<typename Container::value_type>> {
  using ValueType = typename Container::value_type;
};
template <typename Container>
struct ValueTypeImpl<Container, Void<typename adaptor::ContainerAdaptor<Container>::ValueType>> {
  using ValueType = typename adaptor::ContainerAdaptor<Container>::ValueType;
};
template <typename Container>
using ValueType = typename ValueTypeImpl<Container>::ValueType;

}  // namespace type_traits

}  // namespace detail

template <typename Container>
class Flov {
 public:
  using ValueType = detail::type_traits::ValueType<Container>;

  auto push_back(const ValueType& value)
      -> decltype(std::declval<Container>().push_back(value)) {
    Container::push_back(value);
    
  }
};

class X {
 public:
  char& push_back(const char& c) {
    ch = c;
    return ch;
  }
  char ch;
};

template <>
class adaptor::ContainerAdaptor<X> {
 public:
  using ValueType = char;
};

class Y {};

#define DEFINE_HAS_TYPE_MEMBER_TRAIT(TypeMemberName)                   \
  template <typename T, typename = void>                               \
  struct HasTypeMember##TypeMemberName : std::false_type {};           \
                                                                       \
  template <typename T>                                                \
  struct HasTypeMember##TypeMemberName<                                  \
      T, Void<typename T::TypeMemberName>> : std::true_type {}; \
                                                                       \
  template <typename T>                                                \
  /*inline*/ constexpr bool HasTypeMember =                                \
      HasTypeMember##TypeMemberName<T>::value;

template<typename T>
struct Id {
  using type = T;
};

namespace ValueTypeImplTest {

using namespace detail::type_traits;

DEFINE_HAS_TYPE_MEMBER_TRAIT(ValueType /*TypeMemberName*/)

template<typename ...Ts>
using Res = ConditionalApply<HasTypeMemberValueType<Ts...>::value, HasTypeMemberValueType, HasTypeMemberValueType, Ts...>;

static_assert(HasTypeMember<ValueTypeImpl<X>> && IsSame<typename Flov<X>::ValueType, char>,
              "X has ContainerAdaptor specialization.");
    
static_assert(!Res<Y>::value, "Y has neither |::value_type| nor ContainerAdaptor specialization.");
    
//static_assert(HasTypeMember<ValueTypeImpl<Y>>, ".");

static_assert(HasTypeMember<ValueTypeImpl<vector<int>>> && IsSame<typename Flov<vector<int>>::ValueType, int>, "vector<int> has |::value_type| type member.");

}  // namespace ValueTypeImplTest

int main() {
  cout << typeid(Flov<X>::ValueType).name() << endl;
  // cout << typeid(Flov<Y>::ValueType).name() << endl;
  cout << typeid(Flov<vector<int>>::ValueType).name() << endl;
  return 0;
}
