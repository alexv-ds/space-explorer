#include <memory>
#include <cstddef>
#include <atomic>
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <engine/module/main_window.hpp>
#include <engine/Engine.hpp>
#include <spdlog/spdlog.h>

namespace engine {

namespace {
  //internal component
  struct SFML_Window {
    std::shared_ptr<sf::Window> p_window;
    std::size_t event_counter = 0; //+1 при получении ивента с окна
  };
}

inline static main_window::VideoMode video_mode_cast(sf::VideoMode mode) {
  return {
    .width = mode.size.x,
    .height = mode.size.y,
    .bits_per_pixel = mode.bitsPerPixel
  };
}

static void ogl_debug_callback(GLenum source,
                               GLenum type, 
                               GLuint id, 
                               GLenum severity, 
                               GLsizei length, 
                               const GLchar* message, 
                               const void* userParam)
{
  auto source_str = [source]() -> std::string {
    switch (source)
	  {
	    case GL_DEBUG_SOURCE_API: return "API";
	    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
	    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
	    case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
	    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
	    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "UNKNOWN";
	  }
  }();

  auto type_str = [type]() {
	  switch (type)
	  {
	    case GL_DEBUG_TYPE_ERROR: return "ERROR";
	    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
	    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
	    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
	    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
	    case GL_DEBUG_TYPE_MARKER:  return "MARKER";
	    case GL_DEBUG_TYPE_OTHER: return "OTHER";
      default: return "UNKNOWN";
	  }
  }();

  auto severity_str = [severity]() {
	  switch (severity) {
	    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
	    case GL_DEBUG_SEVERITY_LOW: return "LOW";
	    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
	    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
      default: return "UNKNOWN";
	  }
  }();

  SPDLOG_DEBUG("{}, {}, {}, {}: {}", source_str, type_str, severity_str, id, message);
}

MainWindow::MainWindow(flecs::world& world) {
  using namespace main_window;

  //Эти проверки нужны что бы избавитсья от SFML заголовка в main_window.hpp
  //Значения моих вариантов енумов должны совпадать с SFML-ловскими
  static_assert(static_cast<decltype(sf::Style::None)>(Style::None) == sf::Style::None);
  static_assert(static_cast<decltype(sf::Style::Titlebar)>(Style::Titlebar) == sf::Style::Titlebar);
  static_assert(static_cast<decltype(sf::Style::Resize)>(Style::Resize) == sf::Style::Resize);
  static_assert(static_cast<decltype(sf::Style::Close)>(Style::Close) == sf::Style::Close);
  static_assert(static_cast<decltype(sf::Style::Fullscreen)>(Style::Fullscreen) == sf::Style::Fullscreen);
  static_assert(static_cast<decltype(sf::Style::Default)>(Style::Default) == sf::Style::Default);

  world.module<MainWindow>("main_window");

  //VideoMode, RequestDesktopMode, RequestFullscreenModes, RequestCompleted
  world.component<VideoMode>()
    .member<unsigned int>("width")
    .member<unsigned int>("height")
    .member<unsigned int>("bits_per_pixel");
  world.component<RequestDesktopMode>()
    .member<VideoMode>("mode");
  world.component<RequestFullscreenModes>();
  world.component<RequestCompleted>();

  //systems
  world.system<>("Handle_RequestDesctopMode")
    .kind<MainThread_Pre>()
    .term<main_window::RequestDesktopMode>().inout(flecs::In)
    .term<main_window::RequestCompleted>().oper(flecs::Not)
    .each([](flecs::entity e) {
      e.add<main_window::RequestCompleted>();
      e.set<main_window::RequestDesktopMode>({
        .mode = video_mode_cast(sf::VideoMode::getDesktopMode())
      });
    });
  world.system<>("Handle_RequestFullscreenModes")
    .kind<MainThread_Pre>()
    .term<main_window::RequestFullscreenModes>().inout(flecs::In)
    .term<main_window::RequestCompleted>().oper(flecs::Not)
    .each([](flecs::entity e) {
      for (const sf::VideoMode& mode : sf::VideoMode::getFullscreenModes()) {
        flecs::entity relation_entity = e.world().entity()
          .set<main_window::VideoMode>(video_mode_cast(mode))
          .child_of(e);
        e.add(relation_entity);
      }
      e.add<main_window::RequestCompleted>();
    });

  //open and close window
  world.component<Init>();
  world.component<Opened>();
  world.component<Title>();
  world.component<SFML_Window>(); //internal
  world.component<OpenglContext>()
    .member<unsigned int>("depth_bits")
    .member<unsigned int>("stencil_bits")
    .member<unsigned int>("antialiasing_level")
    .member<unsigned int>("ogl_major_version")
    .member<unsigned int>("ogl_minor_version")
    .member<bool>("debug")
    .member<bool>("srgb");
  world.component<Size>()
    .member<unsigned int>("width")
    .member<unsigned int>("height");

  world.component<OpenglViewport>()
    .member<int>("x")
    .member<int>("y")
    .member<int>("width")
    .member<int>("height");
  
  //systems
  world.system<const Init>("InitMainWindow")
    .kind<MainThread_Pre>()
    .term<SFML_Window>().singleton().oper(flecs::Not)
    .each([](flecs::entity e, const Init& init) {
      flecs::world world = e.world();
      std::shared_ptr<sf::Window> p_window;

      //Часть первая, основная инициализация
      {
        sf::VideoMode video_mode({init.video_mode.width, init.video_mode.height}, init.video_mode.bits_per_pixel);
        video_mode.size.x = init.video_mode.width;
        video_mode.size.y = init.video_mode.height;
        video_mode.bitsPerPixel = init.video_mode.bits_per_pixel;

        sf::ContextSettings context;
        context.depthBits = init.depth_bits;
        context.stencilBits = init.stencil_bits;
        context.antialiasingLevel = init.antialiasing_level;
        context.majorVersion = init.ogl_major_version;
        context.minorVersion = init.ogl_minor_version;
        context.sRgbCapable = init.srgb;
        context.attributeFlags = sf::ContextSettings::Attribute::Core;
        if (init.debug) {
          context.attributeFlags |= sf::ContextSettings::Attribute::Debug;
        }


        //Мы так можем сделать, потому что сделали статические проверки. См. начало модуля.
        auto style = static_cast<decltype(sf::Style::Default)>(init.style);
        Title* p_title = world.get_mut<Title>();

        //Инициализируем окешко, ня-ха-ха ^_^
        p_window = std::make_shared<sf::Window>(video_mode, p_title->title, style, context);
        world.set<SFML_Window>({.p_window = p_window});
        e.remove<Init>();
        if (p_window->hasFocus()) {
          world.add<OnFocus>();
        }
      }

      //Вторая часть инициализациия. Выставляем остальные компоненты
      {        
        world.add<Opened>();

        sf::Vector2u window_size = p_window->getSize();
        world.set<Size>({
          .width = window_size.x,
          .height = window_size.y
        });
        
        //Инициализируем glad
        //Защита от двойной инициализации
        static std::atomic_flag glad_inited;
        assert(glad_inited.test_and_set() == false);
        if (!gladLoadGL()) {
          SPDLOG_CRITICAL("Failed to initialize glad");
          assert(false);
        }
        sf::ContextSettings context = p_window->getSettings();
        world.set<OpenglContext>({
          .depth_bits = context.depthBits,
          .stencil_bits = context.stencilBits,
          .antialiasing_level = context.antialiasingLevel,
          .ogl_major_version = context.majorVersion,
          .ogl_minor_version = context.minorVersion,
          .debug = (context.attributeFlags & sf::ContextSettings::Attribute::Debug) ? true : false,
          .srgb = context.sRgbCapable
        });
        OpenglViewport viewport = {
          .x = 0,
          .y = 0,
          .width = (int)(window_size.x), 
          .height = (int)(window_size.y)
        };
        world.set<OpenglViewport>(viewport);
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
      }
      
      //Инициализируем дебажные штуки
      if (init.debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(ogl_debug_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }

    });

    world.component<OnClose>();
    world.component<OnFocus>();
    world.system<SFML_Window>("PollEvents")
      .kind<MainThread_Pre>()
      .term<SFML_Window>().singleton()
      .each([] (flecs::entity window_entity, SFML_Window& window) {
        flecs::world world = window_entity.world();

        sf::Event sf_event;
        while (window.p_window->pollEvent(sf_event)) {
          if (sf_event.type == sf::Event::Closed) {
            world.add<OnClose>();
          } else if (sf_event.type == sf::Event::Resized) {
            world.set<Size>({.width = sf_event.size.width, .height = sf_event.size.height});
            OpenglViewport viewport = {
              .x = 0,
              .y = 0,
              .width = (int)(sf_event.size.width), 
              .height = (int)(sf_event.size.height)
            };
            world.set<OpenglViewport>(viewport);
            glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
          } else if (sf_event.type == sf::Event::GainedFocus) {
            world.add<OnFocus>();
          } else if (sf_event.type == sf::Event::LostFocus) {
            world.remove<OnFocus>();
          }
        }
      });

    world.component<CloseLock>();
    world.system("CloseWindowHandle")
      .kind<MainThread_Pre>()
      .term<OnClose>().singleton()
      .oper(flecs::And)
      .term<Opened>().singleton()
      .iter([](flecs::iter it) {
        if (!it.world().filter<const CloseLock>().is_true()) {
          it.world().remove<Opened>();
          it.world().remove<SFML_Window>();
          it.world().remove<OnClose>();
        }
      });

    world.component<DestroyWorldIfClosed>();
    world.system("DestroyWorld")
      .term<DestroyWorldIfClosed>().singleton()
      .term<Opened>().oper(flecs::Not).singleton()
      .term<Init>().oper(flecs::Not).singleton()
      .iter([](flecs::iter it) {
        it.world().quit();
      }); 

    world.system("Window Swap Buffers")
      .kind<RenderThread>()
      .term<SFML_Window>().singleton()
      .term<OpenglContext>().singleton()
      .iter([](flecs::iter it) {
        const SFML_Window* p_window = it.world().get<SFML_Window>();
        p_window->p_window->display();
      });

}

}

