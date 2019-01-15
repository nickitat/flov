#pragma once

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