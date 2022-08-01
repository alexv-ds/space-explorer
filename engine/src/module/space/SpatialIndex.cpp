#include "SpatialIndex.hpp"

namespace engine::space::internal {

std::shared_ptr<SpatialIndex> SpatialIndex::create_index() {
  return std::shared_ptr<SpatialIndex>(new SpatialIndex);
}



}