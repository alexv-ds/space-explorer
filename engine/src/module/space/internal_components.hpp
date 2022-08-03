#pragma once
#include <glm/vec2.hpp>
#include "SpatialIndex.hpp"
#include "TreeObject.hpp"

namespace engine::space::internal {

struct WorldTreeObject {
  std::shared_ptr<TreeObject> p_object;
};

struct UpdateTreeObject {};
struct LoadIntersections {};
struct BBoxSize {
  float x;
  float y;
};

struct WorldSpatialIndex {
  std::shared_ptr<SpatialIndex> p_index;
  flecs::query<> q_for_init;
  //flecs::query<Object, BBoxSize> q_for_update;
};


}