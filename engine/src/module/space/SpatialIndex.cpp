#include "SpatialIndex.hpp"

namespace engine::space::internal {

std::shared_ptr<SpatialIndex> SpatialIndex::create_index() {
  return std::shared_ptr<SpatialIndex>(new SpatialIndex);
}

SpatialIndex::object_ptr SpatialIndex::create_object() {
  TreeObject* p_object = new TreeObject();
  //p_object->index_lock(shared_from_this());
  //return SpatialIndex::object_ptr(p_object, TreeObjectDeleter());
}

}