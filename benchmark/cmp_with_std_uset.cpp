#include <flov.hpp>

#include <benchmark/benchmark.h>

#include <random>
#include <unordered_set>
#include <vector>

template <class Container>
struct Adaptor;

template <>
struct Adaptor<Flov> {
  void Insert(int key) {
    ds.PushBack(key);
  }

  Flov ds;
};

template <>
struct Adaptor<std::unordered_set<int>> {
  void Insert(int key) {
    ds.insert(key);
  }

  std::unordered_set<int> ds;
};

std::vector<int> GenerateRandomKeys(size_t amount) {
  std::random_device rd;
  std::mt19937 g{rd()};

  std::unordered_set<int> keys;
  keys.reserve(amount);
  while (keys.size() < amount) {
    keys.insert(g());
  }
  return std::vector<int>{keys.begin(), keys.end()};
}

template <class Container, uint32_t N>
void BM_InsertNRandomKeys(benchmark::State& state) {
  const auto keys = GenerateRandomKeys(N);

  for (auto _ : state) {
    Container container;
    for (const auto& key : keys) {
      container.Insert(key);
    }
  }
}

constexpr uint32_t N = 123'456;

BENCHMARK_TEMPLATE(BM_InsertNRandomKeys, Adaptor<Flov>, N);
BENCHMARK_TEMPLATE(BM_InsertNRandomKeys, Adaptor<std::unordered_set<int>>, N);
