#pragma once

#include "SpatialBase.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stack>

/// https://github.com/utec-ads-2020-2/binary-search-tree-otreblan
template <typename key_t, typename T>
class BSTree;

template <typename key_t, typename T>
class Node {
public:
  key_t key;
  T data;

private:
  Node<key_t, T>* parent = nullptr;
  Node<key_t, T>* left = nullptr;
  Node<key_t, T>* right = nullptr;

  template<typename, typename>
  friend class BSTree;

  template<typename, typename>
  friend class Iterator;

  void kill() noexcept
  {
    if(left != nullptr)
      left->kill();

    if(right != nullptr)
      right->kill();

    delete this;
  }
};

template <typename key_t, typename T>
class Iterator
{
  using node_t = Node<key_t, T>;

  std::stack<int> parents;

  node_t* node;

  template<typename, typename>
  friend class BSTree;

  template<typename>
  friend class RangeBST;
public:

  Iterator<key_t, T>():
    node(nullptr)
  {}

  Iterator<key_t, T>(node_t* _node)
  {
    for(node_t* l_node = _node; l_node != nullptr; l_node = l_node->left)
    {
      parents.push(0);
      node = l_node;
    }
  }

  Iterator<key_t, T>(const Iterator<key_t, T>& other) noexcept:
    node(other.node),
    parents(other.parents)
  {};

  bool operator!=(const Iterator<key_t, T>& other) const noexcept
  {
    return node != other.node;
  }

  void up()
  {
    do
    {
      node = node->parent;
      parents.pop();

      // In place completed
      if(parents.empty())
        break;

      parents.top()++;
    }
    while(parents.top() > 1 || node->left == nullptr);
  }

  void down_right()
  {
    node = node->right;
    parents.push(0);

    while(node->left != nullptr)
    {
      parents.push(0);
      node = node->left;
    }
  }

  Iterator<key_t, T>& operator++()
  {
    if(node != nullptr)
    {
      if(node->right != nullptr)
        down_right();
      else
        up();
    }

    return *this;
  }

  std::pair<int, T> operator*() noexcept
  {
    if(node == nullptr)
    {
      std::cerr << "hello: Invalid key\n";
      raise(SIGSEGV);
    }

    return {node->key, node->data};
  }
};

template <typename key_t, typename T>
class BSTree
{
  using node_t     = Node<key_t, T>;
  using iterator_t = Iterator<key_t, T>;

  node_t* root;
  size_t _size;

  node_t* find(key_t key) noexcept
  {
    node_t* node = root;

    while(node != nullptr)
    {
      if(key == node->key)
        return node;
      else if(key > node->key)
        node = node->right;
      else
        node = node->left;
    }

    return nullptr;
  }

  node_t* insert_node(node_t* node, key_t key, const T& data) noexcept
  {
    if(node == nullptr)
    {
      node_t* child = new(std::nothrow) node_t;
      if(child != nullptr)
      {
        child->data = data;
        child->key = key;
        _size++;
      }
      return child;
    }

    if(key == node->key)
    {
      node->data = data;
    }
    else if(key < node->key)
    {
      node->left = insert_node(node->left, key, data);
      if(node->left != nullptr)
        node->left->parent = node;
    }
    else
    {
      node->right = insert_node(node->right, key, data);
      if(node->right != nullptr)
        node->right->parent = node;
    }

    return node;
  }

  node_t* delete_node(node_t* node, key_t key) noexcept
  {
    if(node == nullptr)
      return nullptr;

    if(key < node->key)
    {
      node->left = delete_node(node->left, key);
      if(node->left != nullptr)
        node->left->parent = node;
    }
    else if(key > node->key)
    {
      node->right = delete_node(node->right, key);
      if(node->right != nullptr)
        node->right->parent = node;
    }
    else
    {
      node_t* child = nullptr;

      if(node->left == nullptr)
        child = node->right;
      else if(node->right == nullptr)
        child = node->left;

      if(child != nullptr || (!node->left && !node->right))
      {
        if(node == root)
          root = child;

        if(child != nullptr)
          child->parent = node->parent;

        delete node;
        _size--;
        return child;
      }

      node_t* min_node = nullptr;

      for(node_t* _node = node->right; _node != nullptr; _node = _node->left)
      {
        min_node = _node;
      }

      if(min_node == nullptr)
      {
        std::cerr << "hello: Why\n";
        raise(SIGSEGV);
      }

      std::swap(node->data, min_node->data);
      node->key = min_node->key;

      node->right = delete_node(node->right, min_node->key);
    }

    return node;
  }

  static void pre_order(const node_t* node) noexcept
  {
    if(node == nullptr)
      return;

    std::cout << node->data << '\n';
    pre_order(node->left);
    pre_order(node->right);
  }

  static void in_order(const node_t* node) noexcept
  {
    if(node == nullptr)
      return;

    in_order(node->left);
    std::cout << node->data << '\n';
    in_order(node->right);
  }

  static void post_order(const node_t* node) noexcept
  {
    if(node == nullptr)
      return;

    post_order(node->left);
    post_order(node->right);
    std::cout << node->data << '\n';
  }

  static size_t node_height(const node_t* node) noexcept
  {
    if(node == nullptr)
      return 0;

    return std::max(node_height(node->left)+1, node_height(node->right)+1);
  }

public:
  BSTree() noexcept:
    root(nullptr),
    _size(0)
  {};

  bool insert(key_t key, const T& data) noexcept
  {
    node_t* node = insert_node(root, key, data);

    if(root == nullptr)
      root = node;
    return node;
  }

  bool remove(key_t key) noexcept
  {
    // TODO: Removes an element with a certain key
    return delete_node(root, key);
  }

  bool hasKey(key_t key) noexcept
  {
    return find(key) != nullptr;
  }

  T& operator[](key_t key) noexcept
  {
    // TODO: Gets the element with a certain key and update
    // TODO: It can also insert or update a node (like insert, e.g. tree[2] = "hola")
    // NOTE: Create a different function or modify the method signature if necessary

    node_t* node = nullptr;

    while(!(node = find(key)))
    {
      insert_node(root, key, T());
    }

    return node->data;
  }

  size_t size() const noexcept
  {
    // TODO: Amount of nodes
    return _size;
  }

  size_t height() const noexcept
  {
    return node_height(root);
  }

  bool empty() const noexcept
  {
    return _size == 0;
  }

  void traversePreOrder() const noexcept
  {
    pre_order(root);
  }

  void traverseInOrder() const noexcept
  {
    in_order(root);
  }

  void traversePostOrder() const noexcept
  {
    post_order(root);
  }

  iterator_t begin() noexcept
  {
    return root;
  }

  iterator_t end() noexcept
  {
    return {};
  }

  iterator_t lower_bound(key_t key) noexcept
  {
    if(root == nullptr)
      return {};

    iterator_t it;

    it.node = root;
    it.parents.push(0);

    while(!(it.node->data == key))
    {
      node_t* next_node;
      int next_top;

      if(key < it.node->data)
      {
        // Left
        next_node = it.node->left;
        next_top = 0;
      }
      else
      {
        // Right
        next_node = it.node->right;
        next_top = 1;
      }

      if(next_node == nullptr)
      {
        it.parents.push(1);
        while(it.node->data < key)
          ++it;
        break;
      }

      it.node = next_node;
      it.parents.push(next_top);
    }

    return it;
  }

  ~BSTree() noexcept
  {
    if(root != nullptr)
      root->kill();
  }
};

namespace utec {
namespace spatial {

/**
 * RangeBST implementation
 */
template <typename Point>
class RangeBST : public SpatialBase<Point> {
 private:
   BSTree<Point, Point> tree;

 public:
  RangeBST(){};
  void insert(const Point& new_point) override
  {
    tree.insert(new_point, new_point);
  }

  // El punto de referencia no necesariamente es parte del dataset
  Point nearest_neighbor(const Point& reference) override { return Point({0}); }
  std::vector<Point> range(const Point& min, const Point& max) override {
    std::vector<Point> points;

    for(auto it = tree.lower_bound(min); it != tree.end() && (it.node->key < max || it.node->key == max); ++it)
    {
      points.push_back(it.node->data);
    }

    return points;
  };
};

}  // namespace spatial
}  // namespace utec

// vim: sw=2 et :
