#pragma once

namespace engine::space::internal {

class TreeObject;

class TreeObjectDeleter {
  void operator()(TreeObject*);
};

}