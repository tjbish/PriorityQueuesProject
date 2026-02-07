#include "../include/pq/PairingHeap.hpp"
#include <iostream>

int main() {
  PairingHeap<int> h1;
  PairingHeap<int> h2;

  h2.push(5);
  auto handle_two = h2.push(2);
  h2.push(6);

  h1.push(1);
  h1.push(3);
  h1.push(4);

  h1.meld(h2);

  std::cout << h1.top() << "\n"; // 1
  h1.pop();
  std::cout << h1.top() << "\n"; // 2

  h1.decrease_key(handle_two, 0);
  std::cout << h1.top() << "\n"; // 0
}
