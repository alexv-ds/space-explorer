#pragma once
#include <memory>
#include <vector>
#include <concurrentqueue.h>
#include <engine/utils/noncopyable.hpp>
#include "TreeObject.hpp"

namespace engine::space::internal {

class TreeObjectDeleter;

class SpatialIndex final : public std::enable_shared_from_this<SpatialIndex>, public utils::noncopyable {
public:
  static std::shared_ptr<SpatialIndex> create_index();

  std::shared_ptr<TreeObject> create_object();
  void process_queues();
  ~SpatialIndex();

  void intersects_query(std::vector<flecs::entity_t>& out, const glm::vec2 center, const glm::vec2 size);
private:
  struct Pimpl;
  friend class TreeObject;
  friend class TreeObjectDeleter;

  void destroy_object(TreeObject*);
  void update_object(TreeObject*);
  
  moodycamel::ConcurrentQueue<TreeObject*> insert_queue;
  moodycamel::ConcurrentQueue<TreeObject*> destroy_queue;
  std::unique_ptr<Pimpl> pimpl;

  SpatialIndex();
};


}