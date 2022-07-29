#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <flecs.h>


namespace engine {

struct Fs {
  Fs(flecs::world&);
};

}


namespace engine::fs {

struct FileReader {
  std::string path;
  std::vector<std::uint8_t> data;
};

//запускает читалку
struct FileReaderStart {}; 

//Когда FileReader завершает чтение, этот компонент вешается на объект
//Пользователь может свободно удалять его
struct FileReaderDone {};
struct FileReaderError {};

struct FileReaderWatchdog {
  std::uintmax_t modification_timestamp;
};
//Если установить на объект, то FileReaderWatchdog будет перезагружать файл
//когда пользователь возьмет в фокус окно программы (тобишь развернет его, либо кликнет)
struct Watchdog_RunOnFocus {};

};