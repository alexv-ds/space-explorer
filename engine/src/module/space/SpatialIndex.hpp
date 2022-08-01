#pragma once
#include <memory>
#include <concurrentqueue.h>
#include <engine/utils/noncopyable.hpp>
#include "TreeObject.hpp"

namespace engine::space::internal {

class SpatialIndex final : public std::enable_shared_from_this<SpatialIndex>, public utils::noncopyable {
public:
  using object_ptr = std::shared_ptr<TreeObject>;

  static std::shared_ptr<SpatialIndex> create_index();


  //not thread safe
  void update_object(TreeObject*);
  //thread safe functions
  object_ptr create_object();
private:
  void destroy_object(TreeObject*);
  
  moodycamel::ConcurrentQueue<object_ptr> insert_queue;
  moodycamel::ConcurrentQueue<object_ptr> destroy_queue;

  SpatialIndex() = default;

};


}