#pragma once

#include <constructible_from/constructible_from.hpp>
#include <type_traits/type_traits.hpp>
#include <type_traits/type_utilities.hpp>

#include <type_traits>

#include <limits.h>  // CHAR_BIT
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

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
    std::fill(std::begin(rlink), std::end(rlink), BLink::NPos);
    std::fill(std::begin(match), std::end(match), BLink::NPos);
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

template <class Node>
inline constexpr bool KeysDifferInBit(const Node& lhs,
                                      const Node& rhs,
                                      uint8_t bit) {
  return ((lhs.key >> bit) & 1) != ((rhs.key >> bit) & 1);
}

template <class Node>
inline constexpr bool KeysMatchInBit(const Node& lhs,
                                     const Node& rhs,
                                     uint8_t bit) {
  return ((lhs.key >> bit) & 1) == ((rhs.key >> bit) & 1);
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
      Node newNode{key};
      const BLink newPos{static_cast<BLink::DataType>(nodes.size())};

      for (uint8_t bit = 0; bit < B; ++bit) {
        const auto matchesInAllLowerBits =
            bit ? newNode.match[bit - 1] : BLink{newPos - 1};
        if (!IsValid(matchesInAllLowerBits))
          continue;

        const auto differsInCurrentBit =
            KeysDifferInBit(newNode, nodes[matchesInAllLowerBits], bit)
                ? matchesInAllLowerBits
                : nodes[matchesInAllLowerBits].rlink[bit];

        if (IsValid(differsInCurrentBit)) {
          newNode.rlink[bit] = differsInCurrentBit;
        }

        newNode.match[bit] =
            KeysMatchInBit(newNode, nodes[matchesInAllLowerBits], bit)
                ? matchesInAllLowerBits
                : nodes[matchesInAllLowerBits].rlink[bit];

        auto currentToBeLinkedWithNewNode = differsInCurrentBit;
        while (IsValid(currentToBeLinkedWithNewNode) &&
               !IsValid(nodes[currentToBeLinkedWithNewNode].link[bit])) {
          nodes[currentToBeLinkedWithNewNode].link[bit] = newPos;
          currentToBeLinkedWithNewNode =
              nodes[currentToBeLinkedWithNewNode].match[bit];
          __statistics.numberOfEstablishedLinks++;
        }
      }
      nodes.push_back(std::move(newNode));
    } else {
      nodes.emplace_back(key);
    }
  }

  SizeType Find(KeyType key) {
    Node newNode{key};
    FLink current{};
    for (int bit = 0; bit < B && IsValid(current); ++bit) {
      if (nodes[current].key == key)
        return current;
      if (KeysDifferInBit(newNode, nodes[current], bit)) {
        current = nodes[current].link[bit];
      }
    }
    return IsValid(current) ? static_cast<SizeType>(current) : Size();
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
  std::vector<Node> nodes;

 private:
  struct Statistics {
    uint64_t numberOfEstablishedLinks = 0;
  } __statistics;

  friend void TestInsertNRandomKeysThenSearchForThem(const int);
};
