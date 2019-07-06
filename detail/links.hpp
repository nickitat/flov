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

using Position = cf::MakeUnique<
    class PositionTag,
    cf::ConstructibleFrom<uint32_t,
                          cf::Signature<NonnarrowingConvertibleToInt>>::Type>;

}  // namespace links

}  // namespace detail
}  // namespace flov
