#pragma once
#include <glm/vec2.hpp>
#include "SpatialIndex.hpp"
#include "TreeObject.hpp"

namespace engine::space::internal {

struct WorldSpatialIndex {
  std::shared_ptr<SpatialIndex> p_index;
};

struct WorldTreeObject {
  std::shared_ptr<TreeObject> p_object;
};

struct UpdateTreeObject {};
struct LoadIntersections {};
struct BBoxSize : glm::vec2 {};
  
}