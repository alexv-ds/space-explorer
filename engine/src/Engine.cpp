#include <thread>
#include <engine/Engine.hpp>

namespace engine {

static void RunSystems(flecs::iter& it) {
  for (auto i : it) {
    flecs::system system = it.world().system(it.entity(i));
    system.run();
  }
}

static void execute_systems(flecs::world& world, const flecs::query<> q) {
  world.defer_begin();
  q.iter(RunSystems);
  world.defer_end();
}

Engine::Engine() {
  this->p_world->import<flecs::monitor>();
  this->p_world->set<flecs::Rest>({.port = 0});
  this->p_world->set_target_fps(30.0f);
  this->p_world->set_threads(std::thread::hardware_concurrency());
  this->p_world->component<MainThread_Pre>("::engine::MainThread_Pre");
  this->p_world->component<MainThread_Post>("::engine::MainThread_Post");
  this->p_world->component<RenderThread>("::engine::RenderThread");

  this->preframe_systems_query = this->p_world->query_builder()
    .term<MainThread_Pre>()
    .term(flecs::System)
    .build();
  this->postframe_systems_query = this->p_world->query_builder()
    .term<MainThread_Post>()
    .term(flecs::System)
    .build();
  this->render_systems_query = this->p_world->query_builder()
    .term<RenderThread>().read()
    .term<RenderSystemPriority>().read()
    //.order_by([](flecs::entity e1, const RenderSystemPriority* p1, flecs::entity e2, const RenderSystemPriority* p2) {})
    .term(flecs::System)
    .build();
}

void Engine::run() {
  do {
    float delta_time = this->p_world->frame_begin();

    execute_systems(*this->p_world, this->preframe_systems_query);
    this->p_world->run_pipeline(this->p_world->get_pipeline(), delta_time);
    execute_systems(*this->p_world, this->postframe_systems_query);
    execute_systems(*this->p_world, this->render_systems_query);

    this->p_world->frame_end();
  } while(!this->p_world->should_quit());
}


}