#pragma once

#define DEFINE_HAS_TYPE_MEMBER_TRAIT(TypeMemberName)                           \
  template <typename T, typename = void>                                       \
  struct HasTypeMember##TypeMemberName##Impl : std::false_type {};             \
                                                                               \
  template <typename T>                                                        \
  struct HasTypeMember##TypeMemberName##Impl<T,                                \
                                             Void<typename T::TypeMemberName>> \
      : std::true_type {};                                                     \
                                                                               \
  template <typename T>                                                        \
  /*inline*/ constexpr bool HasTypeMember##TypeMemberName =                    \
      HasTypeMember##TypeMemberName##Impl<T>::value;
