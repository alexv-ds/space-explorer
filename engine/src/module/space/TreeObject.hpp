#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <flecs.h>
#include <engine/utils/noncopyable.hpp>

namespace engine::space::internal {

class TreeObject final : public utils::noncopyable {
public:
  glm::vec2 position; //object center
  glm::vec2 size;
  flecs::entity_t holder;

  void update();
  ~TreeObject();
private:
  friend class BBoxExtractor;
  friend class SpatialIndex;
  friend class TreeObjectDeleter;

  TreeObject();
  

  inline void update_bbox() noexcept;
  void index_lock(std::shared_ptr<SpatialIndex>) noexcept;
  void index_unlock() noexcept;

  float left;
  float top;
  float width;
  float height;

  std::shared_ptr<SpatialIndex> p_index;
};

inline void TreeObject::update_bbox() noexcept {
  left = position.x - size.x / 2.0f;
  top = position.y + size.y / 2.0f;
  width = size.x;
  height = size.y;
}
  
}