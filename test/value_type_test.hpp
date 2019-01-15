#pragma once

#include "../flov.hpp"

#include "dummy_types.hpp"
#include "util/macros.hpp"

#include <vector>

namespace ValueTypeImplTest {

using namespace detail::type_traits;

DEFINE_HAS_TYPE_MEMBER_TRAIT(ValueType)

static_assert(
    HasTypeMemberValueType<
        ValueTypeImpl<TypeWithContainerAdaptorSpecialization>> &&
        IsSame<typename Flov<TypeWithContainerAdaptorSpecialization>::ValueType,
               char>,
    "Case of a type with ContainerAdaptor specialization.");

static_assert(
    !HasTypeMemberValueType<ValueTypeImpl<TypeWithUndefinedValueType>>,
    "Y has neither |::value_type| nor ContainerAdaptor specialization.");

static_assert(HasTypeMemberValueType<ValueTypeImpl<std::vector<int>>> &&
                  IsSame<typename Flov<std::vector<int>>::ValueType, int>,
              "vector<int> has |::value_type| type member.");

}  // namespace ValueTypeImplTest
