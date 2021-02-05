#include <iostream>
#include "SegmentTree.h"

constexpr int(*un)(int, int) = [](int a, int b){ return std::max(a, b); };

int main() {

  algo::SegmentTree<int, un> st(0, 100);
  st[5] = -100;
  st[0] = 5;
  std::cout << st.Get(2, 5) << " " << st.Get(0, 5);
  return 0;
}
