#pragma once

#include <vector>

namespace utec {
namespace spatial {

template <typename Point>
class SpatialBase {
 public:
  virtual void insert(const Point& new_point) = 0;
  //   virtual bool search(Point target) = 0;
  virtual Point nearest_neighbor(const Point& reference) = 0;
  virtual std::vector<Point> range(const Point& min, const Point& max) = 0;
  //   virtual std::vector<Point> k_nearest_neighbor(Point reference,
  //                                                 std::size_t k) = 0;
};

}  // namespace spatial
}  // namespace utec
