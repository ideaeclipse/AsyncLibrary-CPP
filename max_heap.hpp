#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include <utility>
#include <string>
#include <iostream>

namespace ideaeclipse_utils {
/*
 * Simple max heap implementation.
 *
 * Theory of the heap implmentation states you should resize the array everytime you insert / delete an element.
 * You could compare this implmentation to do that but with the typical "ArrayList" or std::vector but instead with
 * an internal implmentation of that data structure.
 *
 * This data strucutre contains a Complete Binary Tree (CBT) which follows the definition of:
 * a CBT is a tree in which every level, except possible the last is completely filled and all nodes are as far left as possible.
 *
 *
 * Functions Time Complexity (Big Theta):
 *
 * Insert: O(logn)
 *
 * Max: O(1)
 *
 * Extract_Max: O(logn)
 */
template<typename T>
class max_heap {
private:
  struct node {
    int priority;
    T data;

    bool operator>(const node &n) {
      return this->priority > n.priority;
    }
  };

  const std::size_t default_size = 50;

  std::size_t array_size = default_size;

  std::size_t heap_size = 0;

  node *nodes[50];

  int get_left_child(const int index) {
    return index >= 0 ? ((index + 1) << 1) - 1 : 0;
  }

  int get_right_child(const int index) {
    return index >= 0 ? (index + 1) << 1 : 0;
  }

  int get_parent(const int index) {
    return index > 0 ? ((index + 1) >> 1) - 1 : 0;
  }

  void drip_up(const int index) {

    if(index == 0) return;

    node **current = &this->nodes[index];

    int parent_index = this->get_parent(index);

    node **parent = &this->nodes[parent_index];

    if((*current)->priority > (*parent)->priority) {
      node *temp = *parent;
      *parent = *current;
      *current = temp;
      drip_up(parent_index);
    }
  }

  void drip_down(const int index) {
    node **current = &this->nodes[index];

    int lc = this->get_left_child(index);
    int rc = this->get_right_child(index);

    int best;
    node **best_node;

    if(lc < this->heap_size && rc < this->heap_size) {
      node *lc_node = this->nodes[lc];
      node *rc_node = this->nodes[rc];

      best = lc_node->priority >= rc_node->priority ? lc : rc;

      best_node = &this->nodes[best];
    } else if(lc < this->heap_size) {
      best = lc;
      best_node = &this->nodes[best];
    } else if(rc < this->heap_size) {
      best = rc;
      best_node = &this->nodes[best];
    } else return;

    if((*best_node)->priority > (*current)->priority) {
      node *temp = *best_node;
      *best_node = *current;
      *current = temp;
      drip_down(best);
    }

  }

public:

  max_heap(const std::size_t initial_capacity) : array_size(initial_capacity) {
    for(node *n : this->nodes) n = nullptr;
  }

  max_heap() : max_heap(default_size) {}

  void insert(const node &new_node) {
    node *n = new node();
    n->data = new_node.data;
    n->priority = new_node.priority;

    this->nodes[this->heap_size] = n;
    drip_up(this->heap_size);
    this->heap_size++;
  }

  const node& max() {
    return *this->nodes[0];
  }

  const node extract_max() {
    node *n = this->nodes[0];

    node copy = {n->priority, n->data};

    delete n;
    n = nullptr;

    this->nodes[0] = this->nodes[--this->heap_size];

    this->nodes[this->heap_size] = nullptr;

    this->drip_down(0);

    return copy;

  }

  void print_tree(const int index, const std::string tab = "") {
    if(index >= this->heap_size) return;
    const node* n = this->nodes[index];
    std::cout << tab << "Node{" << n->priority << "}\n";

    int lc = this->get_left_child(index);

    if(lc < this->heap_size) {
      std::cout << tab << "Left Node:\n";
      print_tree(lc, tab + "\t");
    }

    int rc = this->get_right_child(index);

    if(rc < this->heap_size) {
      std::cout << tab << "Right Node:\n";
      print_tree(rc, tab + "\t");
    }
  }

  void print() {
    std::string message;

    for(int i = 0; i < this->heap_size; i++)
      std::cout << this->nodes[i]->priority << " ";
    std::cout << "\n";
    print_tree(0);
  }

  ~max_heap() {
    for(int i = 0; i < this->heap_size; i ++) {
      delete this->nodes[i];
    }
  }

};
}

#endif
