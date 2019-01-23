#pragma once

namespace adaptor {

// Usage example:
//
// class X {};
//
// template <>
// class ContainerAdaptor<X> {
//  public:
//   using Iterator = char*;
//   using ValueType = char;
// };

template <typename Container>
class ContainerAdaptor {};

}  // namespace adaptor