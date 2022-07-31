cmake_minimum_required(VERSION 3.22.1)
project(Game VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(/W3)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
endif()

find_package(FLECS REQUIRED)
find_package(FMT REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(GLAD REQUIRED)
find_package(SFML REQUIRED)
find_package(JSON REQUIRED)
find_package(GLM REQUIRED)
find_package(LOOSEQUADTREE REQUIRED)


add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

add_executable(main
  src/main.cpp
  src/setup_spdlog.cpp
  engine/src/Engine.cpp
  engine/src/module/time.cpp
  engine/src/module/main_window.cpp
  engine/src/module/fs.cpp
)
target_include_directories(main PRIVATE engine/include)

target_link_libraries(main
  flecs::flecs_static
  fmt::fmt
  spdlog::spdlog
  SFML::SFML
  glad::glad
  nlohmann_json::nlohmann_json
  glm::glm
  loose_quadtree::loose_quadtree
)