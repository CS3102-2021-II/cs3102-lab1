#pragma once

#include "SpatialBase.h"

namespace utec {
namespace spatial {

/**
 * RangeBST implementation
 */
template <typename Point>
class RangeBST : public SpatialBase<Point> {
 private:
  struct node
  {
    Point data;
    node* left = nullptr;
    node* right = nullptr;

    node(const Point& point):
      data(point)
    {};
  };

  node* root = nullptr;

 public:
  RangeBST(){};
  void insert(const Point& new_point) override
  {
    if(root == nullptr)
      root = new node(new_point);
    else
    {
      node* parent = root;
      node* children = new node(new_point);
      //TODO
    }
  }

  // El punto de referencia no necesariamente es parte del dataset
  Point nearest_neighbor(const Point& reference) override { return Point({0}); }
  std::vector<Point> range(const Point& min, const Point& max) override {
    return {};
  };
};

}  // namespace spatial
}  // namespace utec

// vim: sw=2 et :
