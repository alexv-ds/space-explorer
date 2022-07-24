#include <chrono>
#include <engine/module/time.hpp>

namespace engine {

namespace {
  //internal
  struct WorldInitTimepoint {
    std::chrono::steady_clock::time_point start_point = std::chrono::high_resolution_clock::now();
  };
}

time::time(flecs::world& world) {
  static_assert(std::chrono::high_resolution_clock::is_steady == true); //не ну а вдруг нет >:[

  world.import<flecs::units>();
  world.module<time>();

  world.component<Timer>()
    .member<float, flecs::units::duration::Seconds>("timeout")
    .member<float, flecs::units::duration::Seconds>("repeat")
    .member<float, flecs::units::duration::Seconds>("duein");
  world.component<TimerEvent>();
  world.component<Clock>()
    .member<float, flecs::units::duration::Seconds>("time")
    .member<float, flecs::units::duration::Seconds>("delta");
  
  //internal component
  world.component<WorldInitTimepoint>("internal::WorldInitTimepoint");

  //init singleton components
  world.add<WorldInitTimepoint>();
  world.set<Clock>({
      .time = 0.0f
  });

  //systems
  world.system<Clock>("systems::ClockUpdate")
    .kind(flecs::PreFrame)
    .term<Clock>().singleton()
    .term<WorldInitTimepoint>().singleton()
    .each([](flecs::entity e, Clock& clock) {
      const WorldInitTimepoint* p_init_timepoint = e.world().get<WorldInitTimepoint>();
      auto now = std::chrono::high_resolution_clock::now();

      float prev_time = clock.time;
      clock.time = std::chrono::duration<float>(now - p_init_timepoint->start_point).count();
      clock.delta = clock.time - prev_time;
    });
  
  world.observer<Timer>("observers::TimerOnSet")
    .event(flecs::OnAdd).oper(flecs::Or)
    .event(flecs::OnSet).oper(flecs::Or)
    .term<Clock>().singleton()
    .each([](flecs::entity e, Timer& timer) {
      const Clock* p_clock = e.world().get<Clock>();
      timer.duein = p_clock->time + timer.timeout;
    });

  world.system<Timer>("systems::TimerCheck")
    .kind(flecs::OnLoad)
    .term<Clock>().singleton()
    .each([](flecs::entity e, Timer& timer) {
      const Clock* p_clock = e.world().get<Clock>();
      if (timer.duein <= p_clock->time) {
        e.add<TimerEvent>();
        if (timer.repeat > 0) {
          timer.duein = p_clock->time + timer.repeat;
        } else {
          e.remove<Timer>();
        }
      }
    });
  

}

}