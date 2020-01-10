#include "../max_heap.hpp"
#include <iostream>

int main(void) {
  ideaeclipse_utils::max_heap<std::string> heap;
  heap.insert({3, "one"});
  heap.insert({4, "one"});
  heap.insert({5, "one"});
  heap.insert({7, "one"});
  heap.insert({7, "one"});
  heap.insert({9, "one"});
  heap.insert({9, "one"});
  heap.insert({12, "one"});
  heap.insert({17, "one"});
  heap.print();

  for(int i = 0; i < 9; i++) {
    std::cout << "Next: " << heap.extract_max().priority << std::endl;
    //heap.print();
  }

  std::cout << "Done" << std::endl;
  return 0;
}
