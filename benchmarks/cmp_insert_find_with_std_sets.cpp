#include <flov.hpp>

#include <benchmark/benchmark.h>

#include <random>
#include <set>
#include <unordered_set>
#include <vector>

namespace {

template <class Container>
struct Adaptor;

template <>
struct Adaptor<flov::Flov<>> {
  void Insert(int key) {
    ds.PushBack(key);
  }

  void Find(int key) {
    ds.Find(key);
  }

  flov::Flov<> ds;
};

template <>
struct Adaptor<std::set<int>> {
  void Insert(int key) {
    ds.insert(key);
  }

  void Find(int key) {
    ds.find(key);
  }

  std::set<int> ds;
};

template <>
struct Adaptor<std::unordered_set<int>> {
  void Insert(int key) {
    ds.insert(key);
  }

  void Find(int key) {
    ds.find(key);
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

std::vector<int> GenerateRandomPermutation(size_t length) {
  std::random_device rd;
  std::mt19937 g{rd()};

  std::vector<int> permutation(length);
  iota(permutation.begin(), permutation.end(), 0);
  std::shuffle(permutation.begin(), permutation.end(), g);
  return permutation;
}

}  // namespace

template <class Container>
void BM_InsertNRandomKeys(benchmark::State& state) {
  const auto N = state.range(0);
  const auto keys = GenerateRandomKeys(N);

  for (auto _ : state) {
    state.PauseTiming();
    Container container;
    for (const auto& key : keys) {
      container.Insert(key);
    }
    state.ResumeTiming();
    for (auto index : GenerateRandomPermutation(N)) {
      container.Find(keys[index]);
    }
  }
}

BENCHMARK_TEMPLATE(BM_InsertNRandomKeys, Adaptor<flov::Flov<>>)
    ->Range(16, 1 << 20);
BENCHMARK_TEMPLATE(BM_InsertNRandomKeys, Adaptor<std::set<int>>)
    ->Range(16, 1 << 20);
BENCHMARK_TEMPLATE(BM_InsertNRandomKeys, Adaptor<std::unordered_set<int>>)
    ->Range(16, 1 << 20);
