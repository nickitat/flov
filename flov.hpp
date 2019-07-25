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
    Position chain{};
    Position posInChain{};
    uint8_t bit = 0;
  };

 public:
  Node() noexcept : key(-1231) {
  }

  Node(KeyType key) noexcept : key(key) {
  }

  //   void InsertNewLink(Position to, uint8_t bit) {
  // #ifdef FLOV_DEBUG_BUILD
  //     links.PushBack({to, bit});
  // #else
  //     links.PushBack({to});
  // #endif
  //     static constexpr KeyType one = static_cast<KeyType>(1);
  //     FLOV_ASSERT((linksMask & (one << bit)) == 0);
  //     linksMask |= (one << bit);
  //     RestoreLinksOrder(GetIndexOfBit(linksMask, bit));
  //   }

  void InsertNewLink(Position chain, Position posInChain, uint8_t bit) {
    links.PushBack(Link{chain, posInChain, bit});
    linksMask |= (1 << bit);
    RestoreLinksOrder();
  }

 private:
  void RestoreLinksOrder(/*uint8_t index*/) noexcept {
    // FLOV_ASSERT(index < links.Size());
    // for (uint8_t i = links.Size() - 1; i > index; --i)
    //   std::swap(links[i], links[i - 1]);
    for (uint8_t i = links.Size() - 1; i > 0; --i)
      if (links[i].bit < links[i - 1].bit)
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

template <class _KeyType, uint8_t _Bits, class _Position>
struct Chain {
  VectorWithStaticBuffer<Node<_KeyType, _Bits, _Position>, 5> nodes;
};

}  // namespace detail

template <class KeyType = int>
class Flov {
  static constexpr uint32_t B = CHAR_BIT * sizeof(KeyType);
  using Node = detail::Node<KeyType, B, detail::links::Position>;
  using Link = typename Node::Link;
  using Position = typename Node::Position;
  using Nodes = std::vector<Node>;

  using Chain = detail::Chain<KeyType, B, detail::links::Position>;

 public:
  using SizeType = typename Position::DataType;

  void FLOV_NOINLINE PushBack(KeyType key) {
    // std::cerr << "PushBack: " << std::bitset<32>(key).to_string() <<
    // std::endl;
    if (!chains.empty()) {
      // nodes.reserve(nodes.size() + 1); // brings strong EG
      auto&& [chain, posInChain, bit] = FindEx(key);
      // std::cerr << "find res: " << chain << " " << posInChain << " "
      //           << (uint32_t)bit << std::endl;
      // FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size());
      // FLOV_ASSERT(nodes[nodeWithLongestMatchingPrefix].key != key);
      // FLOV_ASSERT(bit < B);
      auto chainToInsert = chains[chain].nodes[posInChain].links.Size() > 0
                               ? (Position) static_cast<SizeType>(chains.size())
                               : chain;
      if (chains[chain].nodes[posInChain].links.Size() > 0)
        chains.emplace_back();
      const auto posToInsert =
          chains[chain].nodes[posInChain].links.Size() > 0
              ? Position{}
              : (Position) static_cast<SizeType>(chains[chain].nodes.Size());
      // const auto newElementPos = Size();
      chains[chain].nodes[posInChain].InsertNewLink(chainToInsert, posToInsert,
                                                    bit);
      // nodes[nodeWithLongestMatchingPrefix].InsertNewLink(newElementPos, bit);
      chains[chainToInsert].nodes.PushBack(Node{key});
    } else {
      Chain chain;
      chain.nodes.PushBack(Node{key});
      chains.push_back(chain);
    }
    // nodes.emplace_back(key);
  }

  const Node* Find(KeyType key) const {
    // std::cerr << "Find: " << std::bitset<32>(key).to_string() << std::endl;
    auto&& [chain, posInChain, bit] = FindEx(key);
    // std::cerr << " " << chain << " " << posInChain << " " << bit <<
    // std::endl; FLOV_ASSERT(nodeWithLongestMatchingPrefix < Size() && bit <=
    // B);
    return chains[chain].nodes[posInChain].key == key
               ? &chains[chain].nodes[posInChain]
               : nullptr;
  }

  SizeType Size() const {
    return static_cast<SizeType>(chains.size());
  }

  // const KeyType& operator[](SizeType index) const {
  //   FLOV_ASSERT(index < nodes.size());
  //   return nodes[index].key;
  // }

  // KeyType& operator[](SizeType index) {
  //   FLOV_ASSERT(index < nodes.size());
  //   return nodes[index].key;
  // }

 private:
  // node with longest matching prefix; first bit where they differ
  Link FLOV_NOINLINE FindEx(KeyType key) const {
    // std::cerr << "FindEx:" << std::endl;
    Position chain{};
    Position posInChain{};
    while (true) {
      // std::cerr
      //     << std::bitset<32>(chains[chain].nodes[posInChain].key).to_string()
      //     << " " << chain << " " << posInChain << std::endl;
      const auto keysXor = key ^ chains[chain].nodes[posInChain].key;
      if (keysXor) {
        const uint8_t firstMismatchingBit = __builtin_ctz(keysXor);
        // std::cerr << "firstMismatchingBit: " << (uint32_t)firstMismatchingBit
        //           << std::endl;
        // std::cerr << "linksMask: " <<
        // chains[chain].nodes[posInChain].linksMask
        //           << std::endl;
        FLOV_ASSERT(firstMismatchingBit < B);
        static constexpr KeyType one = static_cast<KeyType>(1);
        if (chains[chain].nodes[posInChain].linksMask &
            (one << firstMismatchingBit)) {
          const auto index = detail::GetIndexOfBit(
              chains[chain].nodes[posInChain].linksMask, firstMismatchingBit);
          // std::cerr << "index: " << (uint32_t)index << std::endl;
          FLOV_ASSERT(index < chains[chain].nodes[posInChain].links.Size());
          FLOV_ASSERT(chains[chain].nodes[posInChain].links[index].bit ==
                      firstMismatchingBit);
          int ch = chain;
          chain = chains[chain].nodes[posInChain].links[index].chain;
          posInChain = chains[ch].nodes[posInChain].links[index].posInChain;
        } else {
          return {chain, posInChain, firstMismatchingBit};
        }
      } else {
        return {chain, posInChain, B};
      }
    }
  }

 private:
  // std::vector<Node> nodes;
  std::vector<Chain> chains;

 private:
  friend void ::TestInsertNRandomKeysThenSearchForThem(const int);
};  // namespace flov

}  // namespace flov
