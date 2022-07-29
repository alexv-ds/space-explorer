#pragma once
#include <memory>
#include <flecs.h>
#include "utils/noncopyable.hpp"

namespace engine {

struct MainThread_Pre {};
struct MainThread_Post {};
struct RenderThread {};
struct RenderSystemPriority {
  unsigned int priority;
};

class Engine : utils::noncopyable {
public:
  std::shared_ptr<flecs::world> p_world = std::make_shared<flecs::world>();
  Engine();
  void run();
private:
  //для вызова некоторых систем вручную
  flecs::query<> preframe_systems_query;
  flecs::query<> postframe_systems_query;
  flecs::query<> render_systems_query;
};
  
}