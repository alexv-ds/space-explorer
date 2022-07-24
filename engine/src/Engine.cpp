#include <thread>
#include <engine/Engine.hpp>

namespace engine {

static void RunSystems(flecs::iter& it) {
  for (auto i : it) {
    flecs::system system = it.world().system(it.entity(i));
    system.run();
  }
}

Engine::Engine() {
  this->p_world->import<flecs::monitor>();
  this->p_world->set<flecs::Rest>({.port = 0});
  this->p_world->set_target_fps(60.0f);
  this->p_world->set_threads(std::thread::hardware_concurrency());
  this->p_world->component<MainThread_Pre>("::engine::MainThread_Pre");
  this->p_world->component<MainThread_Post>("::engine::MainThread_Post");

  flecs::query_builder systems_query_base = this->p_world->query_builder()
    .term(flecs::System);
  this->preframe_systems_query = systems_query_base.term<MainThread_Pre>().build();
  this->postframe_systems_query = systems_query_base.term<MainThread_Post>().build();
}

void Engine::run() {
  do {
    float delta_time = this->p_world->frame_begin();
    preframe_systems_query.iter(RunSystems);
    this->p_world->run_pipeline(this->p_world->get_pipeline(), delta_time);
    postframe_systems_query.iter(RunSystems);
    this->p_world->frame_end();
  } while(!this->p_world->should_quit());
}


}