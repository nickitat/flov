#pragma once

#include <detail/links.hpp>
#include <detail/macros.hpp>
#include <detail/vector_with_static_buffer.hpp>

#include <limits.h>  // CHAR_BIT
#include <algorithm>
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

inline uint8_t PopCount(int value) noexcept {
  return __builtin_popcount(value);
}

inline uint8_t PopCount(unsigned int value) noexcept {
  return __builtin_popcount(value);
}

inline uint8_t PopCount(long long value) noexcept {
  return __builtin_popcountll(value);
}

inline uint8_t PopCount(unsigned long long value) noexcept {
  return __builtin_popcountll(value);
}

// how many of the preceding bits are set
template <class Mask>
inline uint8_t GetIndexOfBit(Mask mask, uint8_t bit) noexcept {
  const auto correspondingPowerOfTwo = static_cast<Mask>(1) << bit;
  FLOV_ASSERT((mask & correspondingPowerOfTwo) != 0);
  return PopCount(mask & (correspondingPowerOfTwo - 1));
}

template <class KeyType, uint8_t Bits, class PositionT>
class Node {
  static_assert(std::is_arithmetic_v<KeyType>,
                "KeyType should be an arithmetic type");

 public:
  using Position = PositionT;

  struct Link {
    Position to{};
    uint8_t bit = 0;
  };

 public:
  Node(KeyType key) noexcept : key(key) {
  }

  void InsertNewLink(Position to, uint8_t bit) {
#ifdef FLOV_DEBUG_BUILD
    links.PushBack({to, bit});
#else
    links.PushBack({to});
#endif
    static constexpr KeyType one = static_cast<KeyType>(1);
    FLOV_ASSERT((linksMask & (one << bit)) == 0);
    linksMask |= (one << bit);
    RestoreLinksOrder(GetIndexOfBit(linksMask, bit));
  }

 private:
  void RestoreLinksOrder(uint8_t index) noexcept {
    FLOV_ASSERT(index < links.Size());
    for (uint8_t i = links.Size() - 1; i > index; --i)
      std::swap(links[i], links[i - 1]);

#ifdef FLOV_DEBUG_BUILD
    FLOV_ASSERT(
        std::is_sorted(links.Begin(), links.End(),
                       [](auto& lhs, auto& rhs) { return lhs.bit < rhs.bit; }));
#endif
  }

 public:
  KeyType key;  // the key stored

  KeyType linksMask = 0;  // stores bits for which there is a link in |links|
  VectorWithStaticBuffer<Link, 3> links;
};

}  // namespace detail

template <class KeyType = int>
class Flov {
  static constexpr uint32_t B = CHAR_BIT * sizeof(KeyType);
  using Node = detail::Node<KeyType, B, detail::links::Position>;
  using Link = typename Node::Link;
  using Position = typename Node::Position;
  using Nodes = std::vector<Node>;

 public:
  using SizeType = typename Position::DataType;

  void FLOV_NOINLINE PushBack(KeyType key) {
    if (!nodes.empty()) {
      // nodes.reserve(nodes.size() + 1); // brings strong EG
      auto&& [nodeWithLongestMatchingPrefix, bit] = FindEx(key);
      FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size());
      FLOV_ASSERT(nodes[nodeWithLongestMatchingPrefix].key != key);
      FLOV_ASSERT(bit < B);
      const auto newElementPos = Size();
      nodes[nodeWithLongestMatchingPrefix].InsertNewLink(newElementPos, bit);
    }
    nodes.emplace_back(key);
  }

  SizeType Find(KeyType key) const {
    auto&& [nodeWithLongestMatchingPrefix, bit] = FindEx(key);
    FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size() && bit <= B);
    return nodes[nodeWithLongestMatchingPrefix].key == key
               ? static_cast<SizeType>(nodeWithLongestMatchingPrefix)
               : Size();
  }

  SizeType Size() const {
    return static_cast<SizeType>(nodes.size());
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
  Link FLOV_NOINLINE FindEx(KeyType key) const {
    Position current{};
    while (true) {
      const auto keysXor = key ^ nodes[current].key;
      if (keysXor) {
        const uint8_t firstMismatchingBit = __builtin_ctz(keysXor);
        FLOV_ASSERT(firstMismatchingBit < B);
        static constexpr KeyType one = static_cast<KeyType>(1);
        if (nodes[current].linksMask & (one << firstMismatchingBit)) {
          const auto index = detail::GetIndexOfBit(nodes[current].linksMask,
                                                   firstMismatchingBit);
          FLOV_ASSERT(index < nodes[current].links.Size());
          FLOV_ASSERT(nodes[current].links[index].bit == firstMismatchingBit);
          current = nodes[current].links[index].to;
        } else {
          return {current, firstMismatchingBit};
        }
      } else {
        return {current, B};
      }
    }
  }

 private:
  std::vector<Node> nodes;

 private:
  friend void ::TestInsertNRandomKeysThenSearchForThem(const int);
};  // namespace flov

}  // namespace flov
