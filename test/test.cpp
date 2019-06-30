#include "../flov.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include <unordered_map>

using namespace std;

#define ASSERT_FALSE(cond) assert(!(cond))
#define ASSERT_EQ(lhs, rhs) assert((lhs) == (rhs))
#define ASSERT_LE(lhs, rhs) assert((lhs) <= (rhs))

namespace {
struct Timer {
  using Clock = std::chrono::high_resolution_clock;

  Timer() : start(Clock::now()) {
  }

  ~Timer() {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - start);
    cout << "Elapsed time: " << elapsed.count() << " ms" << endl;
  }

  Clock::time_point start;
};

vector<int> GenerateRandomKeys(size_t amount) {
  std::random_device rd;
  std::mt19937 g{rd()};

  std::unordered_set<int> keys;
  keys.reserve(amount);
  while (keys.size() < amount) {
    keys.insert(g());
  }
  return vector<int>{keys.begin(), keys.end()};
}

vector<int> GenerateRandomPermutation(size_t length) {
  std::random_device rd;
  std::mt19937 g{rd()};

  vector<int> permutation(length);
  iota(permutation.begin(), permutation.end(), 0);
  std::shuffle(permutation.begin(), permutation.end(), g);
  return permutation;
}

void ReportMistake(const int dsSize,
                   const int key,
                   const int insertedAt,
                   const size_t foundAt,
                   const int wherePlaced) {
  cout << "Mistake: the key " << key << " ";
  if (foundAt == dsSize) {
    cout << "was not found" << endl;
  } else {
    cout << "inserted at position " << insertedAt << " was found at position "
         << foundAt << " where placed the key " << wherePlaced << endl;
  }
}
}  // namespace

void TestInsertNRandomKeysThenSearchForThem(const int N) {
  const auto keys = GenerateRandomKeys(N);

  Timer timer;

  Flov ds;
  for (const auto& key : keys) {
    ds.PushBack(key);
  }

  bool mistakesFound = false;
  for (auto index : GenerateRandomPermutation(N)) {
    const auto f = ds.Find(keys[index]);
    if (f >= ds.Size() || ds[f] != keys[index]) {
      mistakesFound = true;
      ReportMistake(ds.Size(), keys[index], index, f, ds[f]);
    }
  }

  ASSERT_FALSE(mistakesFound);

  ASSERT_EQ(ds.__statistics.usedLinks.size(), N - 1);

  constexpr auto KeyBitLen = sizeof(int) * CHAR_BIT;
  ASSERT_LE(ds.__statistics.numberOfEstablishedLinks, N * KeyBitLen);
}

int main() {
  for (int i = 0; i < 1000; ++i)
    TestInsertNRandomKeysThenSearchForThem(123'456);
  return 0;
}
