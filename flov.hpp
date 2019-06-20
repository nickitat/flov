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
    type_traits::IsNonnarrowingConvertible<From, int32_t>;

using FLink = MakeUnique<
    class ForwardLink,
    ConstructibleFrom<int32_t, Signature<NonnarrowingConvertibleToInt>>::Type>;

bool IsValid(FLink link) {
  return link >= 0;
}

using BLink = MakeUnique<
    class BackwardLink,
    ConstructibleFrom<int32_t, Signature<NonnarrowingConvertibleToInt>>::Type>;

bool IsValid(BLink link) {
  return link >= 0;
}

static_assert(!std::is_same_v<FLink, BLink>,
              "Two link types should be different types.");
}  // namespace links

template <class Key,
          uint32_t Bits,
          class FLinkT = links::FLink,
          class BLinkT = links::BLink>
class Node {
 public:
  using FLink = FLinkT;
  using BLink = BLinkT;

  Node(Key key) : key(key) {
    std::fill(std::begin(link), std::end(link), -1);
    std::fill(std::begin(rlink), std::end(rlink), -1);
    std::fill(std::begin(match), std::end(match), -1);
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
                      // placed a number having first |i| bits equal to the
                      // first |i| bits of |key|
};

template <class Node>
inline constexpr bool KeysDifferInBit(uint8_t bit,
                                      const Node& lhs,
                                      const Node& rhs) {
  return ((lhs.key >> bit) & 1) != ((rhs.key >> bit) & 1);
}

template <class Node>
inline constexpr bool KeysMatchInBit(uint8_t bit,
                                     const Node& lhs,
                                     const Node& rhs) {
  return ((lhs.key >> bit) & 1) == ((rhs.key >> bit) & 1);
}

}  // namespace detail

class Flov {
  using KeyType = int;
  static constexpr uint32_t B = CHAR_BIT * sizeof(KeyType) + 1;
  using Node = detail::Node<KeyType, B>;
  using FLink = Node::FLink;
  using BLink = Node::BLink;

 public:
  ~Flov() {
    std::cout << "Total number of links established to newly added vertices: "
              << numOfUpdates << std::endl;
  }

  void PushBack(KeyType key) {
    if (!nodes.empty()) {
      Node newNode{key};
      const BLink newPos = static_cast<BLink::DataType>(nodes.size());
      newNode.match[0] = newPos - 1;

      for (int bit = 0; bit < B; ++bit) {
        const auto matchInFirstIPos = newNode.match[bit];
        if (!IsValid(matchInFirstIPos))
          continue;

        const auto diffInPosI =
            KeysDifferInBit(bit, newNode, nodes[matchInFirstIPos])
                ? matchInFirstIPos
                : nodes[matchInFirstIPos].rlink[bit];

        if (IsValid(diffInPosI)) {
          newNode.rlink[bit] = diffInPosI;
        }

        if (bit < B - 1) {
          newNode.match[bit + 1] =
              KeysMatchInBit(bit, newNode, nodes[matchInFirstIPos])
                  ? matchInFirstIPos
                  : nodes[matchInFirstIPos].rlink[bit];
        }

        // nodes[diffInPos0].match[1] - nearest (from the left) node with the
        // same 0-th bit. nodes[nodes[diffInPos0].match[1]].link[0] - link from
        // that node to the nearest (from the right) node with the opposite 0-th
        // bit.
        auto currToBeLinkedWithNewNode = diffInPosI;
        while (IsValid(currToBeLinkedWithNewNode) &&
               !IsValid(nodes[currToBeLinkedWithNewNode].link[bit])) {
          numOfUpdates++;
          nodes[currToBeLinkedWithNewNode].link[bit] =
              static_cast<BLink::DataType>(newPos);
          currToBeLinkedWithNewNode =
              nodes[currToBeLinkedWithNewNode].match[bit + 1];
        }
      }
      nodes.push_back(newNode);
    } else {
      nodes.emplace_back(key);
    }
  }

  size_t Find(KeyType key) {
    Node newNode{key};
    FLink current = 0;
    for (int bit = 0; bit < B && IsValid(current); ++bit) {
      if (nodes[current].key == key)
        return current;
      if (KeysDifferInBit(bit, newNode, nodes[current])) {
        current = nodes[current].link[bit];
      }
    }
    return IsValid(current) ? current : Size();
  }

  size_t Size() const {
    return nodes.size();
  }

  const KeyType& operator[](size_t index) const {
    assert(index < nodes.size());
    return nodes[index].key;
  }

  KeyType& operator[](size_t index) {
    assert(index < nodes.size());
    return nodes[index].key;
  }

 private:
  std::vector<Node> nodes;

  // diagnostic info
  uint64_t numOfUpdates = 0;
};
