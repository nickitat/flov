// #include "iterator_type_test.hpp"
// #include "value_type_test.hpp"

#include "../flov.hpp"

#include <iostream>
#include <vector>

void test() {
  using namespace std;
  static const int N = 123'456;
  vector<int> v(N);
  srand(time(nullptr));
  generate(v.begin(), v.end(), []() { return rand(); });
  Flov ds;
  for (const auto& i : v) {
    ds.PushBack(i);
  }

  vector<int> p(N);
  for (int i = 0; i < N; ++i)
    p[i] = i;
  random_shuffle(p.begin(), p.end());

  bool mistakesFound = false;
  for (const auto& i : p) {
    auto f = ds.Find(v[i]);
    if (f != i) {
      cout << i << " " << f << " " << v[i] << endl;
    }
  }

  const auto logn = 32 - __builtin_clz(N);
  cout << "NlogN = " << N << " * " << logn << " = " << N * logn << endl;
  cout << "Mistakes " << (mistakesFound ? "was" : "wasn't") << " found" << endl;
}

int main() {
  test();
  return 0;
}
