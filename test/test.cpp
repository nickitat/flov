#include "../flov.hpp"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class X {};

template <>
class ContainerAdaptor<X> {
 public:
  using ValueType = vector<char>;
};

class Y {};

int main() {
  cout << typeid(Flov<X>::ValueType).name() << endl;
  // cout << typeid(Flov<Y>::ValueType).name() << endl;
  cout << typeid(Flov<vector<int>>::ValueType).name() << endl;
  return 0;
}
