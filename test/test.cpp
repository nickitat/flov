#include <flov.hpp>

#include <algorithm>
#include <bitset>
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
vector<int> GenerateUniqueRandomKeys(size_t amount) {
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

void ReportMistake(const size_t dsSize,
                   const int key,
                   const int insertedAt,
                   const size_t foundAt,
                   const int wherePlaced) {
  cout << "Mistake: the key " << std::bitset<32>(key).to_string() << " ";
  if (foundAt == dsSize) {
    cout << "was not found" << endl;
  } else {
    cout << "inserted at position " << insertedAt << " was found at position "
         << foundAt << " where placed the key " << wherePlaced << endl;
  }
}
}  // namespace

void TestInsertNRandomKeysThenSearchForThem(const int N) {
  const auto keys = GenerateUniqueRandomKeys(N);

  flov::Flov<> ds;
  ds.nodes.reserve(N);
  for (const auto& key : keys) {
    ds.PushBack(key);
  }

  bool mistakesFound = false;
  for (auto index : GenerateRandomPermutation(N)) {
    const auto f = ds.Find(keys[index]);
    if (f >= ds.Size() || ds[f] != keys[index]) {
      mistakesFound = true;
      if (f < ds.Size())
        ReportMistake(ds.Size(), keys[index], index, f, ds[f]);
      else
        ReportMistake(ds.Size(), keys[index], index, f, 0);
    }
  }

  ASSERT_FALSE(mistakesFound);
}

int main() {
  for (int i = 0; i < 50; ++i)
    TestInsertNRandomKeysThenSearchForThem(123'456);
  return 0;
}
