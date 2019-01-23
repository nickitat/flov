#pragma once

#include "../flov.hpp"

#include "dummy_types.hpp"
#include "util/macros.hpp"

#include <vector>

namespace IteratorTypeImplTest {

using namespace detail::type_traits;

DEFINE_HAS_TYPE_MEMBER_TRAIT(Iterator)

static_assert(
    HasTypeMemberIterator<
        IteratorTypeImpl<TypeWithContainerAdaptorSpecialization>> &&
        IsSame<typename Flov<TypeWithContainerAdaptorSpecialization>::Iterator,
               char*>,
    "Case of a type with ContainerAdaptor specialization.");

static_assert(
    !HasTypeMemberIterator<IteratorTypeImpl<TypeWithUndefinedValueType>>,
    "Y has neither |::iterator| nor ContainerAdaptor specialization.");

static_assert(HasTypeMemberIterator<IteratorTypeImpl<std::vector<int>>> &&
                  IsSame<typename Flov<std::vector<int>>::Iterator,
                         std::vector<int>::iterator>,
              "vector<int> has |::iterator| type member.");

}  // namespace IteratorTypeImplTest
