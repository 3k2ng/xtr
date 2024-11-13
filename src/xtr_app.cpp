#include <xtr_app.h>
namespace xtr {
App::App() : _running{true}, enable_imgui{false} {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    _window =
        SDL_CreateWindow("XToon Renderer", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    _screen_width = 800;
    _screen_height = 600;

    _context = SDL_GL_CreateContext(_window);
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(_window, _context);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}
App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
    IMG_Quit();
    SDL_Quit();
}
void App::update_input() {
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
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (!io.WantCaptureMouse &&
            !(io.WantCaptureKeyboard && io.WantTextInput)) {
            if (event.type == SDL_QUIT) {
                _running = false;
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
                }
            }
        }
    }
}
void App::start_frame() const {
    if (enable_imgui) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}
void App::end_frame() const {
    if (enable_imgui) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    SDL_GL_SwapWindow(_window);
    SDL_Delay(1);
}
} // namespace xtr
