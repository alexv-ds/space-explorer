#pragma once
#include <string>
#include <cstddef>
#include <flecs.h>

namespace engine {

struct MainWindow {
  MainWindow(flecs::world& world);
};

namespace main_window {

  //характеризует размер создаваемого окна
  struct VideoMode {
    unsigned int width = 800;
    unsigned int height = 600;
    unsigned int bits_per_pixel = 32;
  };
  //Запрашивает VideoMode рабочего стола (для полноэкранного окна)
  struct RequestDesktopMode {
    VideoMode mode;
  };
  //Запрашивает поддерживаемые VideoMode для полноэкранного окна
  struct RequestFullscreenModes {};
  //Вешается на объект при завершении запроса
  struct RequestCompleted {};
//////////////////////////////////
  enum class Style {
    None       = 0,      
    Titlebar   = 1 << 0, 
    Resize     = 1 << 1, 
    Close      = 1 << 2, 
    Fullscreen = 1 << 3, 
    Default = Titlebar | Resize | Close 
  };

  struct Init {
    VideoMode video_mode;
    Style style = Style::Default;
    unsigned int depth_bits = 0;
    unsigned int stencil_bits = 0;
    unsigned int antialiasing_level = 0;
    unsigned int ogl_major_version = 4;
    unsigned int ogl_minor_version = 6;
    bool debug = false;
    bool srgb = false;
  };

  /////
  struct Title {
    std::string title;
  };
  struct OpenglContext {
    unsigned int depth_bits = 0;
    unsigned int stencil_bits = 0;
    unsigned int antialiasing_level = 0;
    unsigned int ogl_major_version = 4;
    unsigned int ogl_minor_version = 6;
    bool debug = false;
    bool srgb = false;
  };

  struct OpenglViewport {
    int x;
    int y;
    int width;
    int height;
  };

  struct Size {
    unsigned int width;
    unsigned int height;
  };
  struct Opened {};
  struct OnClose {};
  struct CloseLock {};
  struct DestroyWorldIfClosed {};

  struct OnFocus {};
  




}

}