#pragma once

#include <cmath>
#include <iostream>
#include <stack>
#include <vector>

#include "SpatialBase.h"

namespace utec {
namespace spatial {

template <typename key_t>
class RangeTree;

template <typename key_t>
class Node {
 public:
  key_t key;
  Node<key_t>* right = nullptr;
  Node<key_t>* left = nullptr;

 private:
  template <typename>
  friend class RangeTree;

  void kill() noexcept {
    if (left != nullptr) left->kill();

    if (right != nullptr) right->kill();

    delete this;
  }
};

template <typename key_t>
class RangeTree {
 private:
  using node_t = Node<key_t>;
  node_t* head;
  char lastWeight;

  bool empty() {
    if (head == nullptr) return true;
    return false;
  }

  bool insert_node(node_t* node, key_t key) {
    if (node == nullptr) {
      node_t* newNode = new node_t;
      if (newNode != nullptr) newNode->key = key;
      head = newNode;
      return true;
    } else {
      node_t* temp = node;
      node_t* parent;
      char direction;
      while (temp != nullptr) {
        parent = temp;
        if (key.get(0) > temp->key.get(0)) {
          temp = temp->right;
          direction = 'r';
        } else {
          temp = temp->left;
          direction = 'l';
        }
      }
      node_t* newNode = new node_t;
      if (newNode != nullptr) newNode->key = key;
      temp = newNode;

      node_t* otherNode = new node_t;
      if (direction == 'l') {
        auto value = parent->key;
        parent->key = key;
        otherNode->key = value;
        parent->right = otherNode;
      } else {
        otherNode->key = parent->key;
        parent->left = otherNode;
      }

      balanceTree();
      return true;
    }
  }

  std::vector<key_t> getRange(node_t* node, key_t min, key_t max) {
    node_t* ancient = findAncient(node, min, max);
    std::vector<key_t> leavesLeft = rangeLeft(ancient, min);
    std::vector<key_t> leavesRight = rangeRight(ancient, max);
    std::vector<key_t> range;
    for (auto elem : leavesLeft) {
      range.push_back(elem);
    }
    for (auto elem : leavesRight) {
      range.push_back(elem);
    }
    return range;
  }

  void balanceTree() { calculateWeight(head); }

  void doRightRotationSimple(node_t* node) {
    node_t* temp = node->right;
    node->right = temp->left;
    temp->left = node;
    node = temp;
  }

  void doLeftRotationSimple(node_t* node) {
    node_t* temp = node->left;
    node->left = temp->right;
    temp->right = node;
    node = temp;
  }

  void doLeftRightRotation(node_t* node) {
    doLeftRotationSimple(node->left);
    doRightRotationSimple(node);
  }

  void doRightLeftRotation(node_t* node) {
    doRightRotationSimple(node->right);
    doLeftRotationSimple(node);
  }

  int calculateWeight(node_t* node) {
    int weight;
    int max = 0;
    int left = 0;
    int right = 0;
    if (node->right != nullptr) {
      right = calculateWeight(node->right);
    }
    if (node->left != nullptr) {
      left = calculateWeight(node->left);
    }

    // hallamos el peso
    if (node->left == nullptr && node->right == nullptr) {
      weight = 0;
    } else {
      weight = left - right;
    }

    // balanceamos de acuerdo al signo del peso y del nodo que le sigue
    if (weight <= -2) {
      if (lastWeight == '-')
        doLeftRotationSimple(node);
      else
        doLeftRightRotation(node);
    } else if (weight >= 2) {
      if (lastWeight == '+')
        doRightRotationSimple(node);
      else
        doRightLeftRotation(node);
    }

    // hallamos la cantidad de nodos maximos y guardamos el ultimo signo
    if (right > left) {
      max = right + 1;
      lastWeight = '-';
    } else {
      max = left + 1;
      lastWeight = '+';
    }

    return max;
  }

  node_t* findAncient(node_t* node, key_t min, key_t max) {
    node_t* ancient = node;
    while (true) {
      if (ancient->key.get(0) < min.get(0) && ancient->key.get(0) < max.get(0))
        ancient = ancient->right;
      else if (ancient->key.get(0) > min.get(0) &&
               ancient->key.get(0) > max.get(0))
        ancient = ancient->left;
      else
        return ancient;
    }
  }

  std::vector<key_t> rangeLeft(node_t* ancient, key_t min) {
    node_t* node = ancient->left;
    std::vector<key_t> rangeLeft;
    std::vector<node_t*> nodes;

    while (node != nullptr) {
      if (min.get(0) < node->key.get(0)) {
        nodes.insert(nodes.begin(), node);
        node = node->left;
      } else if (min.get(0) > node->key.get(0)) {
        node = node->right;
      } else {
        if (node->left == nullptr) rangeLeft.push_back(node->key);
        node = node->left;
      }
    }

    for (node_t* nod : nodes) {
      std::vector<key_t> elems = getLeaves(nod->right);
      for (key_t elem : elems) {
        rangeLeft.insert(rangeLeft.begin(), elem);
      }
    }

    return rangeLeft;
  }

  std::vector<key_t> rangeRight(node_t* ancient, key_t max) {
    node_t* node = ancient->right;
    std::vector<key_t> rangeRight;

    while (node != nullptr) {
      if (max.get(0) > node->key.get(0)) {
        std::vector<key_t> elems = getLeaves(node->left);
        for (key_t elem : elems) {
          rangeRight.push_back(elem);
        }
        node = node->right;
      } else if (max.get(0) < node->key.get(0)) {
        node = node->left;
      } else {
        if (node->left == nullptr) rangeRight.push_back(node->key);
        node = node->left;
      }
    }

    return rangeRight;
  }

  std::vector<key_t> getLeaves(node_t* node) {
    std::vector<key_t> leaves;
    if (node->left == nullptr && node->right == nullptr) {
      leaves.push_back(node->key);
    } else {
      if (node->left != nullptr) {
        std::vector<key_t> left = getLeaves(node->left);
        for (key_t elem : left) {
          leaves.push_back(elem);
        }
      }
      if (node->right != nullptr) {
        std::vector<key_t> right = getLeaves(node->right);
        for (key_t elem : right) {
          leaves.push_back(elem);
        }
      }
    }
    return leaves;
  }

 public:
  RangeTree() { head = nullptr; };

  void insert(key_t key) { insert_node(head, key); }

  std::vector<key_t> range(key_t min, key_t max) {
    std::vector<key_t> points;
    points = getRange(head, min, max);
    return points;
  }
};

/**
 * RangeTree1D implementation
 */
template <typename Point>
class RangeTree1D : public SpatialBase<Point> {
 private:
  RangeTree<Point> tree;

 public:
  RangeTree1D(){};
  void insert(const Point& new_point) override { tree.insert(new_point); }

  // El punto de referencia no necesariamente es parte del dataset
  Point nearest_neighbor(const Point& reference) override { return Point({0}); }
  std::vector<Point> range(const Point& min, const Point& max) override {
    return tree.range(min, max);
  };
};

}  // namespace spatial
}  // namespace utec
