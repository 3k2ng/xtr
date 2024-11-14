#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <unordered_map>
namespace xtr {
class App {
  public:
    App(int width, int height);
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;
    ~App();

    inline const bool is_running() const { return _running; }

    void update_input();
    void start_frame() const;
    void end_frame() const;

    inline const bool is_key_pressed(const SDL_Keycode k) {
        return _key_pressed[k];
    }

    inline const bool is_key_down(const SDL_Keycode k) { return _key_down[k]; }

    inline const bool is_key_repeated(const SDL_Keycode k) {
        return _key_repeated[k];
    }

    inline const bool is_button_pressed(const unsigned char b) {
        return _button_pressed[b];
    }

    inline const bool is_button_down(const unsigned char b) {
        return _button_down[b];
    }

    inline const glm::vec2 get_mouse_position() const {
        return _mouse_position;
    }

    inline const glm::vec2 get_mouse_delta() const { return _mouse_delta; }
    inline const glm::vec2 get_wheel_delta() const { return _wheel_delta; }

    inline const int get_screen_width() const { return _screen_width; }
    inline const int get_screen_height() const { return _screen_height; }

    bool enable_imgui;

  private:
    SDL_Window *_window;
    SDL_GLContext _context;

    bool _running;
    std::unordered_map<SDL_Keycode, bool> _key_pressed, _key_down,
        _key_repeated;
    std::unordered_map<unsigned char, bool> _button_pressed, _button_down;
    glm::vec2 _mouse_position, _mouse_delta, _wheel_delta;
    int _screen_width, _screen_height;
};
} // namespace xtr
