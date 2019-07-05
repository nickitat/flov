#pragma once

#include <detail/links.hpp>
#include <detail/macros.hpp>

#include <limits.h>  // CHAR_BIT
#include <cassert>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

void TestInsertNRandomKeysThenSearchForThem(const int);

namespace flov {

namespace detail {

template <class Key, uint8_t Bits, class FLinkT = links::FLink>
// class BLinkT = links::BLink>
class Node {
 public:
  using FLink = FLinkT;
  // using BLink = BLinkT;

  struct Link {
    uint8_t bit;
    FLink link;
  };

  Node(Key key) : key(key) {
    // std::fill(std::begin(link), std::end(link), FLink::NPos);
    // std::fill(std::begin(rlink), std::end(rlink), BLink::NPos);
    // std::fill(std::begin(match), std::end(match), BLink::NPos);
  }

  Key key;  // the key stored

  Key linksMask = 0;
  std::vector<Link> links;

  // FLink link[Bits];  // link[i] - nearest position from the right where
  // placed a number having first |i| bits equal to the
  // first |i| bits of |key| and whose |i|-th bit
  // differs from the |i|-th bit of |key|

  // BLink rlink[Bits];  // rlink[i] - nearest position from the left where
  // placed a number having first |i| bits equal to the
  // first |i| bits of |key| and whose |i|-th bit
  // differs from the |i|-th bit of |key|

  // BLink match[Bits];  // match[i] - nearest position from the left where
  // placed a number having all bits 0...|i| equal to the
  // bits 0...|i| of |key|
};

template <class KeyType>
inline bool KeysDifferInBit(const KeyType lhs, const KeyType rhs, uint8_t bit) {
  return ((lhs >> bit) & 1) != ((rhs >> bit) & 1);
}

template <class KeyType>
inline bool KeysMatchInBit(const KeyType lhs, const KeyType rhs, uint8_t bit) {
  return ((lhs >> bit) & 1) == ((rhs >> bit) & 1);
}

}  // namespace detail

class Flov {
  using KeyType = int;
  static constexpr uint32_t B = CHAR_BIT * sizeof(KeyType);
  using Node = detail::Node<KeyType, B>;
  using FLink = Node::FLink;
  // using BLink = Node::BLink;
  using Nodes = std::vector<Node>;

 public:
  using SizeType = std::common_type_t<FLink::DataType, Nodes::size_type>;

  void FLOV_NOINLINE PushBack(KeyType key) {
    if (!nodes.empty()) {
      auto&& [nodeWithLongestMatchingPrefix, bit] = FindEx(key);
      FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size());
      FLOV_ASSERT(nodes[nodeWithLongestMatchingPrefix].key != key);
      FLOV_ASSERT(bit < B);
      nodes[nodeWithLongestMatchingPrefix].linksMask |= (KeyType)1 << bit;
      auto& links = nodes[nodeWithLongestMatchingPrefix].links;
      links.push_back({bit, static_cast<FLink::DataType>(Size())});
      for (size_t i = links.size() - 1; i > 0; --i) {
        if (links[i].bit < links[i - 1].bit) {
          using std::swap;
          swap(links[i], links[i - 1]);
        } else {
          break;
        }
      }
      FLOV_ASSERT(std::is_sorted(
          links.begin(), links.end(),
          [](auto& lhs, auto& rhs) { return lhs.bit < rhs.bit; }));
    }
    nodes.emplace_back(key);
  }

  SizeType Find(KeyType key) const {
    auto&& [nodeWithLongestMatchingPrefix, bit] = FindEx(key);
    FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size() && bit <= B);
    const bool keysMatch = nodes[nodeWithLongestMatchingPrefix].key == key;
    return keysMatch ? nodeWithLongestMatchingPrefix : Size();
  }

  SizeType Size() const {
    return nodes.size();
  }

  const KeyType& operator[](SizeType index) const {
    FLOV_ASSERT(index < nodes.size());
    return nodes[index].key;
  }

  KeyType& operator[](SizeType index) {
    FLOV_ASSERT(index < nodes.size());
    return nodes[index].key;
  }

 private:
  // node with longest matching prefix; first bit where they differ
  std::pair<FLink, uint8_t> FLOV_NOINLINE FindEx(KeyType key) const {
    FLink current{};
    while (true) {
      if (key == nodes[current].key)
        return {current, B};
      const uint8_t bit = __builtin_ctz(key ^ nodes[current].key);
      FLOV_ASSERT(bit < B);
      if (nodes[current].linksMask & ((KeyType)1 << bit)) {
        const auto mask = ((KeyType)1 << bit) - 1;
        const auto pos = __builtin_popcount(nodes[current].linksMask & mask);
        FLOV_ASSERT(nodes[current].links[pos].bit == bit);
        current = nodes[current].links[pos].link;
        // for (auto& link : nodes[current].links) {
        //   if (link.bit == bit) {
        //     current = link.link;
        //     break;
        //   }
        // }

      } else {
        return {current, bit};
      }
      // maybe here we can do smth like if (link[i].bit > link[i+1].bit)
      // swap(link[i], link[i+1]);
    }

    // if (detail::KeysDifferInBit(key, nodes[current].key, bit)) {
    //   if (IsValid(nodes[current].link[bit])) {
    //     current = nodes[current].link[bit];
    //     FLOV_DEBUG_INFO(
    //         __statistics.MarkLinkAsUsed(current,
    //         nodes[current].link[bit]));
    //   } else
    //     break;
    // }
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

  friend void ::TestInsertNRandomKeysThenSearchForThem(const int);
};  // namespace flov

}  // namespace flov
