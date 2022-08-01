#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <flecs.h>
#include <engine/utils/noncopyable.hpp>

namespace engine::space::internal {

class TreeObject final : public std::enable_shared_from_this<TreeObject>, public utils::noncopyable {
public:
  glm::vec2 position; //object center
  glm::vec2 size;
  flecs::entity_t holder;

  ~TreeObject();
private:
  friend class BBoxExtractor;
  friend class SpatialIndex;

  TreeObject();
  inline void update_bbox() noexcept;

  float left;
  float top;
  float width;
  float height;
};


inline void TreeObject::update_bbox() noexcept {
  left = position.x - size.x / 2.0f;
  top = position.y + size.y / 2.0f;
  width = size.x;
  height = size.y;
}

/*class BBoxExtractor {
  static inline void ExtractBoundingBox(const TreeObject* in, TreeObject::BBox* out) noexcept {
    (*out) = in->bbox;
  }
};*/



  
}