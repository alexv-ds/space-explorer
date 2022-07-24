#include <spdlog/spdlog.h>
#include <engine/module/time.hpp>
#include <engine/engine.hpp>
#include <engine/module/main_window.hpp>

int main(int argc, char const *argv[]) {
  engine::Engine engine;
  engine.p_world->import<engine::time>();

  engine.p_world->entity().set<engine::time::Timer>({.timeout = 5, .repeat = 4});
  engine.p_world->system<const engine::time::Clock>("MySystem")
    .kind<engine::MainThread_Pre>()
    .term<engine::time::Clock>().singleton()
    .each([](flecs::entity e, const engine::time::Clock& clock) {
      SPDLOG_INFO("Boooop");
    });

  engine.run();
  return 0;
}
