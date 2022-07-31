
#include <spdlog/spdlog.h>
#include <engine/engine.hpp>
#include <engine/module/time.hpp>
#include <engine/module/main_window.hpp>
#include <engine/module/fs.hpp>
#include <engine/module/map.hpp>
#include "setup_spdlog.hpp"

int main(int argc, char const *argv[]) {
  setup_spdlog();
  
  engine::Engine engine;
  engine.p_world->import<engine::time>();
  engine.p_world->import<engine::MainWindow>();


  engine.p_world->entity().set<engine::main_window::Init>({});
  engine.p_world->add<engine::main_window::DestroyWorldIfClosed>();


  engine.run();
  return 0;
}
