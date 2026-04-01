#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// Min-heap by default (use Compare to flip to max-heap, etc.)
template <class T, class Compare = std::less<T>>
class PairingHeap {
public:
  struct Node {
    T key;
    Node* child = nullptr;   // leftmost child
    Node* sibling = nullptr; // next sibling
    Node* parent = nullptr;  // needed for decrease_key

    explicit Node(const T& k) : key(k) {}
    explicit Node(T&& k) : key(std::move(k)) {}
  };

  using handle_type = Node*;

  PairingHeap() = default;
  explicit PairingHeap(Compare comp) : comp_(std::move(comp)) {}

  PairingHeap(const PairingHeap&) = delete;
  PairingHeap& operator=(const PairingHeap&) = delete;

  PairingHeap(PairingHeap&& other) noexcept { move_from(std::move(other)); }
  PairingHeap& operator=(PairingHeap&& other) noexcept {
    if (this != &other) {
      clear();
      move_from(std::move(other));
    }
    return *this;
  }

  ~PairingHeap() { clear(); }

  bool empty() const noexcept { return root_ == nullptr; }
  std::size_t size() const noexcept { return size_; }

  const T& top() const {
    if (!root_) throw std::runtime_error("top() on empty heap");
    return root_->key;
  }

  handle_type push(const T& value) {
    Node* n = new Node(value);
    root_ = meld_nodes(root_, n);
    ++size_;
    return n;
  }

  handle_type push(T&& value) {
    Node* n = new Node(std::move(value));
    root_ = meld_nodes(root_, n);
    ++size_;
    return n;
  }

  void pop() {
    if (!root_) throw std::runtime_error("pop() on empty heap");

    Node* old = root_;
    root_ = two_pass_merge(old->child);

    old->child = old->sibling = old->parent = nullptr;
    delete old;
    --size_;

    if (root_) root_->parent = nullptr;
  }

  // Meld (join) other into this; other becomes empty.
  void meld(PairingHeap& other) {
    if (this == &other) return;
    root_ = meld_nodes(root_, other.root_);
    size_ += other.size_;
    other.root_ = nullptr;
    other.size_ = 0;
  }

  // Precondition: new_key must be "better" than current key under Compare.
  void decrease_key(handle_type h, const T& new_key) {
    if (!h) throw std::runtime_error("decrease_key(null handle)");

    if (!comp_(new_key, h->key) && !equivalent_(new_key, h->key)) {
      throw std::runtime_error("decrease_key called with non-decreasing key");
    }

    h->key = new_key;
    if (h == root_) return;

    cut_from_parent(h);
    root_ = meld_nodes(root_, h);
  }

  void clear() noexcept {
    destroy_subtree(root_);
    root_ = nullptr;
    size_ = 0;
  }

private:
  Node* root_ = nullptr;
  std::size_t size_ = 0;
  Compare comp_{};

  bool equivalent_(const T& a, const T& b) const {
    return !comp_(a, b) && !comp_(b, a);
  }

  void move_from(PairingHeap&& other) noexcept {
    root_ = other.root_;
    size_ = other.size_;
    comp_ = std::move(other.comp_);
    other.root_ = nullptr;
    other.size_ = 0;
  }

  static void link_as_child(Node* parent, Node* child) {
    child->parent = parent;
    child->sibling = parent->child;
    parent->child = child;
  }

  Node* meld_nodes(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;

    if (comp_(b->key, a->key)) std::swap(a, b); // a wins
    link_as_child(a, b);
    return a;
  }

  Node* two_pass_merge(Node* first) {
    if (!first) return nullptr;

    std::vector<Node*> merged;
    merged.reserve(8);

    Node* cur = first;
    while (cur) {
      Node* a = cur;
      Node* b = cur->sibling;
      cur = (b ? b->sibling : nullptr);

      a->sibling = nullptr;
      a->parent = nullptr;

      if (b) {
        b->sibling = nullptr;
        b->parent = nullptr;
        merged.push_back(meld_nodes(a, b));
      } else {
        merged.push_back(a);
      }
    }

    Node* res = nullptr;
    for (std::size_t i = merged.size(); i-- > 0;) {
      res = meld_nodes(res, merged[i]);
    }
    return res;
  }

  void cut_from_parent(Node* x) {
    Node* p = x->parent;
    if (!p) return;

    if (p->child == x) {
      p->child = x->sibling;
    } else {
      Node* prev = p->child;
      while (prev && prev->sibling != x) prev = prev->sibling;
      if (!prev) throw std::runtime_error("internal error: cut failed");
      prev->sibling = x->sibling;
    }

    x->parent = nullptr;
    x->sibling = nullptr;
  }

  static void destroy_subtree(Node* n) noexcept {
    std::vector<Node*> stack;
    while (n) {
      stack.push_back(n);
      n = n->sibling;
    }
    while (!stack.empty()) {
      Node* cur = stack.back();
      stack.pop_back();

      Node* c = cur->child;
      while (c) {
        Node* next = c->sibling;
        stack.push_back(c);
        c = next;
      }
      delete cur;
    }
  }
};
