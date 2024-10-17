#include <SDL2/SDL.h>
#include <glad/gl.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <stb_image.h>

#include <xtr_buffer.h>
#include <xtr_shader.h>

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window =
        SDL_CreateWindow("XToon Renderer", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    xtr::Program program{};
    {
        xtr::Shader vsh = xtr::Shader::from_file("./data/shaders/test.vert",
                                                 GL_VERTEX_SHADER);
        vsh.log_compile_status();
        xtr::Shader fsh = xtr::Shader::from_file("./data/shaders/test.frag",
                                                 GL_FRAGMENT_SHADER);
        fsh.log_compile_status();
        program.attach(vsh);
        program.attach(fsh);
        program.link();
        program.log_link_status();
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    xtr::Array array;

    xtr::Buffer vertex_buffer{GL_ARRAY_BUFFER},
        element_buffer{GL_ELEMENT_ARRAY_BUFFER};

    array.bind();
    vertex_buffer.bind();
    vertex_buffer.data(sizeof(vertices), vertices, GL_STATIC_DRAW);

    element_buffer.bind();
    element_buffer.data(sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    vertex_buffer.unbind();

    array.unbind();

    bool xtr_running = true;
    while (xtr_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                xtr_running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("panel");
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, 800, 600);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        array.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
