#pragma once

#include "../include/intf.hpp"

class TypeWithContainerAdaptorSpecialization {
 public:
  char& push_back(const char& c) {
    ch = c;
    return ch;
  }
  char ch;
};

template <>
class adaptor::ContainerAdaptor<TypeWithContainerAdaptorSpecialization> {
 public:
  using Iterator = char*;
  using ValueType = char;
};

// Nor |::value_type| type member nor ContainerAdaptor<> specialization exist
// for this type
class TypeWithUndefinedValueType {};
