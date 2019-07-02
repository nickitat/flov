#pragma once

#include <type_traits>

#include <constructible_from/constructible_from.hpp>
#include <type_traits/type_traits.hpp>
#include <type_traits/type_utilities.hpp>

namespace flov {
namespace detail {

namespace links {
namespace cf = constructible_from;

template <class From>
using NonnarrowingConvertibleToInt =
    type_traits::IsNonnarrowingConvertible<From, uint32_t>;

struct FLink
    : cf::MakeUnique<class ForwardLink,
                     cf::ConstructibleFrom<
                         uint32_t,
                         cf::Signature<NonnarrowingConvertibleToInt>>::Type> {
  using MakeUnique::MakeUnique;
  static constexpr DataType NPos = static_cast<DataType>(-1);
};

bool IsValid(FLink link) {
  return link != FLink::NPos;
}

struct BLink
    : cf::MakeUnique<class BackwardLink,
                     cf::ConstructibleFrom<
                         uint32_t,
                         cf::Signature<NonnarrowingConvertibleToInt>>::Type> {
  using MakeUnique::MakeUnique;
  static constexpr DataType NPos = static_cast<DataType>(-1);
};

bool IsValid(BLink link) {
  return link != BLink::NPos;
}

static_assert(!std::is_same_v<FLink, BLink>,
              "Two link types should be different types.");
}  // namespace links

}  // namespace detail
}  // namespace flov
