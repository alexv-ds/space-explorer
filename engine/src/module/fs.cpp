#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <engine/module/fs.hpp>
#include <engine/module/time.hpp>
#include <engine/module/main_window.hpp>

namespace engine {

namespace {
  struct WatchdogRUN {};

  static void WatchdogRUN_System(flecs::entity e, const fs::FileReaderWatchdog&, const WatchdogRUN&) {
    e.remove<WatchdogRUN>();
    //Нужно снять TimerEvent, ибо если Watchdog в режием фокуса окна, то к моменту срабатывания, уже может быть новый TimerEvent
    //На объекте и из-за этого будет двойное чтение файла
    e.remove<time::TimerEvent>(); 
    e.add<fs::FileReaderStart>();
  };
}

using namespace fs;

Fs::Fs(flecs::world& world) {
  world.import<MainWindow>();
  world.import<Time>();
  world.module<Fs>("fs");

  world.component<FileReader>();
  world.component<FileReaderDone>();
  world.component<FileReaderWatchdog>()
    .member<std::uintmax_t>("modification_timestamp");
  world.component<FileReaderError>();
  world.component<Watchdog_RunOnFocus>();
  world.component<WatchdogRUN>("internal::WatchdogRUN");

  world.system<FileReader>("systems::FileReader")
    .term<FileReaderStart>()
    .term<FileReaderError>().oper(flecs::Not)
    .each([](flecs::entity e, FileReader& reader) {
      SPDLOG_TRACE("Reading... - '{}'", reader.path);
      e.remove<FileReaderStart>();
      try {
        std::ifstream file(reader.path, std::ios::in | std::ios::binary | std::ios::ate);
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        reader.data.assign(size, 0);
        file.read(reinterpret_cast<char*>(reader.data.data()), size);
        if (e.has<FileReaderWatchdog>()) {
          FileReaderWatchdog* p_watchdog = e.get_mut<FileReaderWatchdog>();
          auto last_write = std::filesystem::last_write_time(reader.path);
          p_watchdog->modification_timestamp = last_write.time_since_epoch().count();
        }
        e.add<FileReaderDone>();
      }
      catch(const std::exception& exception) {
        SPDLOG_ERROR("Cannot read '{}' - {}", reader.path, exception.what());
        reader.data.clear();
        e.add<FileReaderError>();
      }
    });

  world.observer<const FileReaderWatchdog>("observers::FileReaderWatchdogAdd")
    .event(flecs::OnAdd)
    .each([](flecs::entity e, const FileReaderWatchdog&) {
      e.set<time::Timer>({.timeout=2.0f, .repeat=2.0});
    });

  world.observer<const FileReaderWatchdog>("observers::FileReaderWatchdog_RemoveCleanup")
    .event(flecs::OnRemove)
    .each([](flecs::entity e, const FileReaderWatchdog&) {
      e.remove<time::Timer>();
      e.remove<WatchdogRUN>();
    });

  world.system<const FileReaderWatchdog, const FileReader, const time::TimerEvent>("systems::WatchdogCheck")
    
    .term<WatchdogRUN>().oper(flecs::Not)
    .term<FileReaderError>().oper(flecs::Not)
    .each([](flecs::entity e, const FileReaderWatchdog& watchdog, const FileReader& reader, const time::TimerEvent&) {
      e.remove<time::TimerEvent>();
      std::error_code err_code;
      auto last_write = std::filesystem::last_write_time(reader.path, err_code);
      if (err_code) {
        SPDLOG_ERROR("Cannot get last write time from '{}' - {}", reader.path, err_code.message());
        return;
      }
      if (watchdog.modification_timestamp != last_write.time_since_epoch().count()) {
        e.add<WatchdogRUN>();
        SPDLOG_TRACE("File was changed, mark to reloading - '{}'", reader.path);
      }
    });

  world.system<const FileReaderWatchdog, const WatchdogRUN>("systems::WatchdogRUN")
    .term<Watchdog_RunOnFocus>().oper(flecs::Not)
    .each(WatchdogRUN_System);

  world.system<const FileReaderWatchdog, const WatchdogRUN>("systems::WatchdogRUN_OnFocus")
    .term<Watchdog_RunOnFocus>()
    .term<main_window::OnFocus>().singleton()
    .each(WatchdogRUN_System);
}

}