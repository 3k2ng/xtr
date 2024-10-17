#include <xtr_app.h>
#include <xtr_buffer.h>
#include <xtr_shader.h>
#include <xtr_texture.h>

struct Vertex {
    float x, y, z;
    float r, g, b;
    float u, v;
};

int main(int argc, char *argv[]) {
    xtr::App app{};

    SDL_Surface *madoka = IMG_Load("./data/textures/madoka.png");

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

    xtr::Texture texture{GL_TEXTURE_2D};
    glBindTexture(texture.target(), texture);
    glTexParameteri(texture.target(), GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture.target(), GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(texture.target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture.target(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(texture.target(), 0, GL_RGBA, madoka->w, madoka->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, madoka->pixels);

    Vertex vertices[] = {
        Vertex{0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        Vertex{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    };
    unsigned int indices[] = {
        0, 1, 3, 1, 2, 3,
    };

    xtr::Array array;

    xtr::Buffer vertex_buffer{GL_ARRAY_BUFFER},
        element_buffer{GL_ELEMENT_ARRAY_BUFFER};

    array.bind();
    vertex_buffer.bind();
    vertex_buffer.data(sizeof(vertices), vertices, GL_STATIC_DRAW);

    element_buffer.bind();
    element_buffer.data(sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vertex_buffer.unbind();

    array.unbind();

    while (app.is_running()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                app.quit();
            }
        }
        app.start_frame();

        ImGui::Begin("panel");
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, 800, 600);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        array.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        app.end_frame();
    }

    SDL_FreeSurface(madoka);
    return 0;
}
