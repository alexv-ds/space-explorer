#include "SpatialIndex.hpp"
#include "TreeObject.hpp"
#include "TreeObjectDeleter.hpp"

namespace engine::space::internal {

void TreeObjectDeleter::operator()(TreeObject* p_object) {
  assert(p_object->p_index != nullptr);
  std::shared_ptr p_index = p_object->p_index;
  p_index->destroy_object(p_object);
}

}