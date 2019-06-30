#pragma once

#include <constructible_from/constructible_from.hpp>
#include <type_traits/type_traits.hpp>
#include <type_traits/type_utilities.hpp>

#include <limits.h>  // CHAR_BIT
#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

#include <unordered_set>

class Flov;

namespace detail {

namespace links {
using namespace constructible_from;

template <class From>
using NonnarrowingConvertibleToInt =
    type_traits::IsNonnarrowingConvertible<From, uint32_t>;

struct FLink
    : MakeUnique<
          class ForwardLink,
          ConstructibleFrom<uint32_t,
                            Signature<NonnarrowingConvertibleToInt>>::Type> {
  using MakeUnique::MakeUnique;
  static constexpr DataType NPos = static_cast<DataType>(-1);
};

bool IsValid(FLink link) {
  return link != FLink::NPos;
}

struct BLink
    : MakeUnique<
          class BackwardLink,
          ConstructibleFrom<uint32_t,
                            Signature<NonnarrowingConvertibleToInt>>::Type> {
  using MakeUnique::MakeUnique;
  static constexpr DataType NPos = static_cast<DataType>(-1);
};

bool IsValid(BLink link) {
  return link != BLink::NPos;
}

static_assert(!std::is_same_v<FLink, BLink>,
              "Two link types should be different types.");
}  // namespace links

template <class Key,
          uint8_t Bits,
          class FLinkT = links::FLink,
          class BLinkT = links::BLink>
class Node {
 public:
  using FLink = FLinkT;
  using BLink = BLinkT;

  Node(Key key) : key(key) {
    std::fill(std::begin(link), std::end(link), FLink::NPos);
    // std::fill(std::begin(rlink), std::end(rlink), BLink::NPos);
    // std::fill(std::begin(match), std::end(match), BLink::NPos);
  }

  Key key;  // the key stored

  FLink link[Bits];  // link[i] - nearest position from the right where
                     // placed a number having first |i| bits equal to the
                     // first |i| bits of |key| and whose |i|-th bit
                     // differs from the |i|-th bit of |key|

  BLink rlink[Bits];  // rlink[i] - nearest position from the left where
                      // placed a number having first |i| bits equal to the
                      // first |i| bits of |key| and whose |i|-th bit
                      // differs from the |i|-th bit of |key|

  BLink match[Bits];  // match[i] - nearest position from the left where
                      // placed a number having all bits 0...|i| equal to the
                      // bits 0...|i| of |key|
};

template <class KeyType>
inline constexpr bool KeysDifferInBit(const KeyType lhs,
                                      const KeyType rhs,
                                      uint8_t bit) {
  return ((lhs >> bit) & 1) != ((rhs >> bit) & 1);
}

template <class KeyType>
inline constexpr bool KeysMatchInBit(const KeyType lhs,
                                     const KeyType rhs,
                                     uint8_t bit) {
  return ((lhs >> bit) & 1) == ((rhs >> bit) & 1);
}

}  // namespace detail

class Flov {
  using KeyType = int;
  static constexpr uint32_t B = CHAR_BIT * sizeof(KeyType);
  using Node = detail::Node<KeyType, B>;
  using FLink = Node::FLink;
  using BLink = Node::BLink;
  using Nodes = std::vector<Node>;

 public:
  using SizeType = Nodes::size_type;

  void PushBack(KeyType key) {
    if (!nodes.empty()) {
      auto&& [lastNode, bit] = FindEx(key);
      assert(nodes[lastNode].key != key && "all the keys should be unique");
      assert(bit < B);
      const auto newPos = nodes.size();
      nodes[lastNode].link[bit] = static_cast<BLink::DataType>(newPos);
    }
    nodes.emplace_back(key);
  }

  SizeType Find(KeyType key) const {
    auto&& [lastNode, bit] = FindEx(key);
    assert(IsValid(lastNode));
    return nodes[lastNode].key == key ? static_cast<SizeType>(lastNode)
                                      : Size();
  }

  SizeType Size() const {
    return nodes.size();
  }

  const KeyType& operator[](SizeType index) const {
    assert(index < nodes.size());
    return nodes[index].key;
  }

  KeyType& operator[](SizeType index) {
    assert(index < nodes.size());
    return nodes[index].key;
  }

 private:
  std::pair<FLink, uint8_t> FindEx(KeyType key) const {
    FLink current{};
    uint8_t bit = 0;
    for (; bit < B; ++bit) {
      if (nodes[current].key == key)
        return {current, bit};
      if (detail::KeysDifferInBit(key, nodes[current].key, bit)) {
        if (IsValid(nodes[current].link[bit])) {
          current = nodes[current].link[bit];
          // __statistics.MarkLinkAsUsed(current, nodes[current].link[bit]);
        } else
          return {current, bit};
      }
    }
    return {current, bit};
  }

  std::vector<Node> nodes;

 private:
  struct Statistics {
    void MarkLinkAsUsed(const FLink& from, const FLink& to) {
      usedLinks.insert(((uint64_t)from << 32) + to);
    }
    uint64_t numberOfEstablishedLinks = 0;
    std::unordered_set<uint64_t> usedLinks;
  };
  mutable Statistics __statistics;

  friend void TestInsertNRandomKeysThenSearchForThem(const int);
};
