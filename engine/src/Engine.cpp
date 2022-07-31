#include <thread>
#include <map>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <engine/Engine.hpp>
#include <engine/module/fs.hpp>

namespace engine {

namespace {
//internal component
struct RenderSystemPriority {
  unsigned int priority;
};

struct RenderSystemPrioritiesList {
  std::map<std::string, unsigned int> priorities;
};

struct RenderQueueLoader {};
struct HasNoPriority {};

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

} //end of anonymous namespace

Engine::Engine() {  
  this->p_world->set<flecs::Rest>({.port = 0});
  this->p_world->set_target_fps(30.0f);
  this->p_world->set_threads(std::thread::hardware_concurrency());
  this->p_world->component<MainThread_Pre>("::engine::MainThread_Pre");
  this->p_world->component<MainThread_Post>("::engine::MainThread_Post");
  this->p_world->component<RenderThread>("::engine::RenderThread");
  this->p_world->component<RenderSystemPriority>("::engine::internal::RenderSystemPriority");
  this->p_world->component<RenderQueueLoader>("::engine::internal::RenderQueueLoader");
  this->p_world->component<RenderSystemPrioritiesList>("::engine::internal::RenderSystemPrioritiesList");
  this->p_world->component<HasNoPriority>("::engine::internal::HasNoPriority");
  

  this->p_world->import<flecs::monitor>();
  this->p_world->import<Fs>();

  this->p_world->entity()
    .set_doc_name("Render Queue Loader")
    .add<RenderQueueLoader>()
    .set<fs::FileReader>({.path="resources/rsystems-execute-queue.jsonc"})
    .add<fs::FileReaderWatchdog>()
    .add<fs::FileReaderStart>();
    

  this->p_world->system<const fs::FileReader>("Engine Queue Loader System")
    .term<fs::FileReaderDone>()
    .term<RenderQueueLoader>()
    .each([](flecs::entity e, const fs::FileReader& reader) {
      e.remove<fs::FileReaderDone>();
      try {
        //парсим json и приводим в нужным нам вид
        std::map<std::string, unsigned int> priorities;
        const nlohmann::json j_data = nlohmann::json::parse(reader.data.begin(), reader.data.end(), nullptr, true, true);
        if (!j_data.is_array()) {
          throw std::logic_error("except array of json data");
        }
        unsigned int counter = 0;
        for (const auto& j_system_name : j_data) {
          if (!j_system_name.is_string()) {
            SPDLOG_WARN("Render queue element was ignored (not a string) - {}", j_system_name.dump());
            continue;
          }
          priorities.insert({j_system_name.get<std::string>(), ++counter});
        }
        e.world().remove_all<RenderSystemPriority>();
        e.world().remove_all<HasNoPriority>();
        e.world().set<RenderSystemPrioritiesList>({.priorities=std::move(priorities)});
      } catch(const std::exception& exception) {
        SPDLOG_CRITICAL("Fail to load render queue - {}", exception.what());
      }
    });

  this->p_world->system("Engine Priority Setter System")
    .term<RenderSystemPrioritiesList>().singleton()
    .term(flecs::System)
    .term<RenderThread>()
    .term<RenderSystemPriority>().oper(flecs::Not)
    .term<HasNoPriority>().oper(flecs::Not)
    .iter([](flecs::iter it) {
      const RenderSystemPrioritiesList* p_list = it.world().get<RenderSystemPrioritiesList>();
      std::string str_buffer;
      for (auto i : it) {
        flecs::entity e = it.entity(i);
        str_buffer.clear();
        str_buffer += e.name();
        if (auto it = p_list->priorities.find(str_buffer); it != p_list->priorities.end()) {
          e.set<RenderSystemPriority>({.priority=it->second});
        } else {
          e.add<HasNoPriority>();
          SPDLOG_ERROR("System '{}' - the system is not find in the priority list", str_buffer);
        }
      }
    }); 

  

  this->preframe_systems_query = this->p_world->query_builder()
    .term<MainThread_Pre>()
    .term(flecs::System)
    .build();
  this->postframe_systems_query = this->p_world->query_builder()
    .term<MainThread_Post>()
    .term(flecs::System)
    .build();
  this->render_systems_query = this->p_world->query_builder()
    .term<RenderThread>()
    .term<RenderSystemPriority>()
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