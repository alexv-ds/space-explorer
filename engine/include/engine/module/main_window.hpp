#pragma once
#include <string>
#include <flecs.h>

struct main_window {

  struct Opened {};
  struct HasFocus {};
  struct OpenglContext {};
  struct MouseCursorVisible {};
  struct MouseCursorGrabbed {};
  struct Size {
    unsigned int x;
    unsigned int y;
  };
  struct SizeEvent {};
  struct Title {
    std::string title;
  };
  struct TitleEvent {};

  struct VSyncEnabled {};
  

};