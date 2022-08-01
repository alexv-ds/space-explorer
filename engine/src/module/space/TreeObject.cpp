#pragma once
#include <cassert>
#include "TreeObject.hpp"
#include "SpatialIndex.hpp"

namespace engine::space::internal {

TreeObject::TreeObject() = default;
TreeObject::~TreeObject() {
  assert(this->p_index == nullptr);
};

void TreeObject::index_lock(std::shared_ptr<SpatialIndex> p_index) noexcept {
  this->p_index = std::move(p_index);
}
void TreeObject::index_unlock() noexcept {
  this->p_index = nullptr;
}

void TreeObject::update() {
  if (this->p_index == nullptr) {
    return;
  }
  update_bbox();
  this->p_index->update_object(this);
}

}