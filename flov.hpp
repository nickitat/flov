#pragma once

#include "include/type_traits.hpp"

#include <type_traits>

namespace lookup_strategy {

class CacheFriendlyBinarySearch {};

}  // namespace lookup_strategy

template <typename Container>
class Flov {
 public:
  using Iterator = detail::type_traits::Iterator<Container>;
  using ValueType = detail::type_traits::ValueType<Container>;

  auto push_back(const ValueType& value)
      -> decltype(std::declval<Container>().push_back(value)) {
    Container::push_back(value);
  }

  auto push_back(ValueType&& value)
      -> decltype(std::declval<Container>().push_back(std::move(value))) {
    Container::push_back(std::move(value));
  }
};
