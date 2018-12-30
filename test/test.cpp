#include "../flov.hpp"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

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
  struct HasTypeMember##TypeMemberName<                                \
      T, std::void_t<typename T::TypeMemberName>> : std::true_type {}; \
                                                                       \
  template <typename T>                                                \
  inline constexpr bool HasTypeMember =                                \
      HasTypeMember##TypeMemberName<T>::value;

namespace ValueTypeImplTest {

using namespace detail::type_traits;

DEFINE_HAS_TYPE_MEMBER_TRAIT(ValueType /*TypeMemberName*/)

static_assert(HasTypeMember<ValueTypeImpl<X>>,
              "X has ContainerAdaptor specialization.");
static_assert(
    !HasTypeMember<ValueTypeImpl<Y>>,
    "Y has neither |::value_type| nor ContainerAdaptor specialization.");
static_assert(HasTypeMember<ValueTypeImpl<vector<int>>>,
              "vector<int> has |::value_type| type member.");

}  // namespace ValueTypeImplTest

int main() {
  cout << typeid(Flov<X>::ValueType).name() << endl;
  // cout << typeid(Flov<Y>::ValueType).name() << endl;
  cout << typeid(Flov<vector<int>>::ValueType).name() << endl;
  return 0;
}
