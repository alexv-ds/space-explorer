#pragma once
#include <memory>
#include <concurrentqueue.h>
#include <engine/utils/noncopyable.hpp>
#include "TreeObject.hpp"

namespace engine::space::internal {

template<class T> class TreeObjectDeleter;

class SpatialIndex final : public std::enable_shared_from_this<SpatialIndex>, public utils::noncopyable {
public:
  using object_ptr = std::shared_ptr<TreeObject>;
  static std::shared_ptr<SpatialIndex> create_index();

  object_ptr create_object();
private:
  friend class TreeObject;
  friend template<class T> class TreeObjectDeleter;

  void destroy_object(TreeObject*) {}
  void update_object(TreeObject*) {}
  
  moodycamel::ConcurrentQueue<object_ptr> insert_queue;
  moodycamel::ConcurrentQueue<object_ptr> destroy_queue;

  SpatialIndex();
  SpatialIndex();
};


}