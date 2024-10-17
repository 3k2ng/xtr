#pragma once
#include <SDL2/SDL.h>
#include <glad/gl.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
namespace xtr {
class App {
  public:
    App();
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;
    ~App();

    inline void quit() { _running = false; }
    inline const bool is_running() const { return _running; }

    inline void end_frame() const {
        SDL_GL_SwapWindow(_window);
        SDL_Delay(1);
    }

  private:
    SDL_Window *_window;
    SDL_GLContext _context;

    bool _running;
};
} // namespace xtr
