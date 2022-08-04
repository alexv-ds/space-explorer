
#include <spdlog/spdlog.h>
#include <engine/engine.hpp>
#include <engine/module/time.hpp>
#include <engine/module/main_window.hpp>
#include <engine/module/fs.hpp>
#include <engine/module/space.hpp>
#include "setup_spdlog.hpp"

#include <glad/glad.h>

struct Test {
  float sign = 1;
};

int main(int argc, char const *argv[]) {
  setup_spdlog();
  
  engine::Engine engine;
  engine.p_world->import<engine::MainWindow>();
  engine.p_world->import<engine::Space>();

  engine.p_world->entity().set<engine::main_window::Init>({});
  engine.p_world->add<engine::main_window::DestroyWorldIfClosed>();

  engine.p_world->entity("TESTENTITYBLED_1")
    .set<engine::space::Object>({.x=1,.y=1})
    .set<engine::space::Position>({});

  engine.p_world->entity("TESTENTITYBLED_3")
    .set<engine::space::Object>({.x=1,.y=1})
    .set<engine::space::Position>({});

  engine.p_world->entity("TESTENTITYBLED_4")
    .set<engine::space::Object>({.x=1,.y=1})
    .set<engine::space::Position>({});

  engine.p_world->entity("TESTENTITYBLED_2")
    .set<engine::space::Object>({.x=1,.y=1})
    .set<engine::space::Position>({})
    .add<engine::space::HandleIntersections>()
    .add<Test>();

  engine.p_world->system<engine::space::Position, Test>("TESTSYSTEM")
    .each([](flecs::entity e, engine::space::Position& pos, Test& test) {
      pos.x += 0.01f * test.sign;
      e.set<engine::space::Position>({
        .x = pos.x += 0.01f * test.sign,
        .y = pos.y
      });
      if (pos.x > 3.0f) {
        test.sign = -1.0f;
      } else if (pos.x < -3.0f) {
        test.sign = 1.0f;
      }
    });

  engine.p_world->system("system::GlClear")
    .kind<engine::RenderThread>()
    .term<engine::main_window::OpenglContext>().singleton()
    .iter([](flecs::iter it) {
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    });


  engine.run();
  return 0;
}
