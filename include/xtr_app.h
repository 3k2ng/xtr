// container for window creation, opengl context creation and input handling
// using SDL
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
    App(int width, int height) : enable_imgui{false}, _window_resized{false} {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

        _window = SDL_CreateWindow("XToon Renderer", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, width, height,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        _screen_width = width;
        _screen_height = height;

        _context = SDL_GL_CreateContext(_window);
        gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(_window, _context);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;
    ~App() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(_context);
        SDL_DestroyWindow(_window);
        IMG_Quit();
        SDL_Quit();
    }

    // check if the window is closing, and also update the input
    inline bool is_running() {
        ImGuiIO &io = ImGui::GetIO();
        SDL_Event event;
        for (auto &[k, v] : _key_pressed) {
            v = false;
        }
        for (auto &[b, v] : _button_pressed) {
            v = false;
        }
        _mouse_delta = {};
        _wheel_delta = {};
        _window_resized = false;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (!io.WantCaptureMouse &&
                !(io.WantCaptureKeyboard && io.WantTextInput)) {
                if (event.type == SDL_QUIT) {
                    return false;
                } else if (event.type == SDL_KEYUP) {
                    const SDL_Keycode k = event.key.keysym.sym;
                    _key_down[k] = false;
                    _key_pressed[k] = false;
                    _key_repeated[k] = false;
                } else if (event.type == SDL_KEYDOWN) {
                    const SDL_Keycode k = event.key.keysym.sym;
                    _key_pressed[k] = !_key_down[k];
                    _key_down[k] = true;
                    _key_repeated[k] = event.key.repeat;
                } else if (event.type == SDL_MOUSEBUTTONUP) {
                    const unsigned char b = event.button.button;
                    _button_down[b] = false;
                    _button_pressed[b] = false;
                    _mouse_position = {static_cast<float>(event.button.x) /
                                           static_cast<float>(_screen_width),
                                       static_cast<float>(event.button.y) /
                                           static_cast<float>(_screen_height)};
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    const unsigned char b = event.button.button;
                    _button_pressed[b] = !_button_down[b];
                    _button_down[b] = true;
                    _mouse_position = {static_cast<float>(event.button.x) /
                                           static_cast<float>(_screen_width),
                                       static_cast<float>(event.button.y) /
                                           static_cast<float>(_screen_height)};
                } else if (event.type == SDL_MOUSEMOTION) {
                    _mouse_position = {static_cast<float>(event.motion.x) /
                                           static_cast<float>(_screen_width),
                                       static_cast<float>(event.motion.y) /
                                           static_cast<float>(_screen_height)};
                    _mouse_delta = {static_cast<float>(event.motion.xrel) /
                                        static_cast<float>(_screen_width),
                                    static_cast<float>(event.motion.yrel) /
                                        static_cast<float>(_screen_height)};
                } else if (event.type == SDL_MOUSEWHEEL) {
                    _wheel_delta = {event.wheel.preciseX, event.wheel.preciseY};
                } else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        _screen_width = event.window.data1;
                        _screen_height = event.window.data2;
                        _window_resized = true;
                    }
                }
            }
        }
        return true;
    }

    // start rendering
    inline void start_frame() const {
        if (enable_imgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
        }
    }

    // end rendering
    inline void end_frame() const {
        if (enable_imgui) {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        SDL_GL_SwapWindow(_window);
        SDL_Delay(1);
    }

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

    inline const bool is_window_resized() const { return _window_resized; }

    inline const int get_screen_width() const { return _screen_width; }
    inline const int get_screen_height() const { return _screen_height; }

    bool enable_imgui;

  private:
    SDL_Window *_window;
    SDL_GLContext _context;

    std::unordered_map<SDL_Keycode, bool> _key_pressed, _key_down,
        _key_repeated;
    std::unordered_map<unsigned char, bool> _button_pressed, _button_down;
    glm::vec2 _mouse_position, _mouse_delta, _wheel_delta;
    bool _window_resized;
    int _screen_width, _screen_height;
};
} // namespace xtr
