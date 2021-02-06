#include <iostream>
#include <climits>
#include "SegmentTree.h"

constexpr int(*un)(int, int) = [](int a, int b){ return std::max(a, b); };
constexpr int(*sum)(int, int) = [](int a, int b){ return a + b; };

int main() {

  algo::SegmentTree<int, un, INT_MIN> st(0, 100);
  st[5] = -100;
  st[0] = 5;
  std::cout << st.Get(2, 5) << " " << st.Get(0, 5) << '\n';
  algo::SegmentTree<int, un, INT_MIN> st2(0, 50);
  st = st2;
  std::cout << st.Get(2, 5) << '\n';
  algo::SegmentTree<int, sum, 0> sm(0, 5);
  for (int i = 0; i < 5; i++) {
    sm[i] = i;
  }
  std::cout << sm.Get(0, 4) << '\n';
  return 0;
}
