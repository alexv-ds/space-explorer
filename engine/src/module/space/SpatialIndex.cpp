#include <LooseQuadtree.h>
#include "SpatialIndex.hpp"
#include "TreeObjectDeleter.hpp"
#include "TreeObject.hpp"

namespace engine::space::internal {

using BBox = loose_quadtree::BoundingBox<float>;

class BBoxExtractor {
public:
  static inline void ExtractBoundingBox(const TreeObject* in, BBox* out) noexcept {
    out->height = in->height;
    out->width = in->width;
    out->left = in->left;
    out->top = in->top;
  }
};

struct SpatialIndex::Pimpl {
  loose_quadtree::LooseQuadtree<float,TreeObject,BBoxExtractor> quad_tree;
};

std::shared_ptr<SpatialIndex> SpatialIndex::create_index() {
  return std::shared_ptr<SpatialIndex>(new SpatialIndex());
}

SpatialIndex::SpatialIndex() {
  this->pimpl = std::make_unique<SpatialIndex::Pimpl>();
}
SpatialIndex::~SpatialIndex() = default;

std::shared_ptr<TreeObject> SpatialIndex::create_object() {
  TreeObject* p_object = new TreeObject();
  p_object->index_lock(shared_from_this());
  std::shared_ptr<TreeObject> ptr(p_object, [](TreeObject* p_object) {
    assert(p_object->p_index != nullptr);
    std::shared_ptr p_index = p_object->p_index;
    p_index->destroy_object(p_object);
  });
  this->insert_queue.enqueue(ptr.get());
  return ptr;
}

void SpatialIndex::destroy_object(TreeObject* p_object) {
  this->destroy_queue.enqueue(p_object);
}

void SpatialIndex::update_object(TreeObject* p_object) {
  this->pimpl->quad_tree.Update(p_object);
}

void SpatialIndex::process_queues() {
  TreeObject* p_object = nullptr;
  while (this->insert_queue.try_dequeue(p_object)) {
    p_object->update();
  }
  while (this->destroy_queue.try_dequeue(p_object)) {
    this->pimpl->quad_tree.Remove(p_object);
    delete p_object;
  }
};

void SpatialIndex::intersects_query(std::vector<flecs::entity_t>& out, const glm::vec2 center, const glm::vec2 size) {
  BBox bbox(
    center.x - size.x / 2.0f,
    center.y + size.y / 2.0f,
    size.x,
    size.y
  );
  loose_quadtree::LooseQuadtree<float,TreeObject,BBoxExtractor>::Query query = this->pimpl->quad_tree.QueryIntersectsRegion(bbox);
  while (!query.EndOfQuery()) {
    out.push_back(query.GetCurrent()->holder);
    query.Next();
  }
}

}