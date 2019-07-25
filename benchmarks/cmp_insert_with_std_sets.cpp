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

  flov::Flov<> ds;
};

template <>
struct Adaptor<std::set<int>> {
  void Insert(int key) {
    ds.insert(key);
  }

  std::set<int> ds;
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

class BM_InsertNRandomKeys : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {
    input = GenerateRandomKeys(state.range(0));
  }

  std::vector<int> input;
};

template <class Container>
void InsertKeys(benchmark::State& state, const std::vector<int>& keys) {
  for (auto _ : state) {
    Container container;
    for (const auto& key : keys) {
      container.Insert(key);
    }
  }
}

}  // namespace

BENCHMARK_DEFINE_F(BM_InsertNRandomKeys, Flov)(benchmark::State& st) {
  InsertKeys<Adaptor<flov::Flov<>>>(st, input);
}
BENCHMARK_REGISTER_F(BM_InsertNRandomKeys, Flov)
    ->RangeMultiplier(2)
    ->Range(16, 1 << 20);

BENCHMARK_DEFINE_F(BM_InsertNRandomKeys, Set)(benchmark::State& st) {
  InsertKeys<Adaptor<std::set<int>>>(st, input);
}
BENCHMARK_REGISTER_F(BM_InsertNRandomKeys, Set)
    ->RangeMultiplier(2)
    ->Range(16, 1 << 20);

BENCHMARK_DEFINE_F(BM_InsertNRandomKeys, USet)(benchmark::State& st) {
  InsertKeys<Adaptor<std::unordered_set<int>>>(st, input);
}
BENCHMARK_REGISTER_F(BM_InsertNRandomKeys, USet)
    ->RangeMultiplier(2)
    ->Range(16, 1 << 20);
