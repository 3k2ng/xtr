#include <xtr_app.h>
#include <xtr_buffer.h>
#include <xtr_camera.h>
#include <xtr_obj.h>
#include <xtr_shader.h>
#include <xtr_texture.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec2 uv;
};

int main(int argc, char *argv[]) {
    xtr::App app{};
    glEnable(GL_DEPTH_TEST);
    xtr::TurnTableCamera camera{5., glm::half_pi<float>(), 0., {}};
    xtr::Mesh cube = xtr::load_obj_file("./data/models/cube.obj");
    glm::mat4 model_matrix = glm::mat4{1.f};

    xtr::Program program = xtr::load_program("./data/shaders/basic.vert",
                                             "./data/shaders/basic.frag");

    xtr::Texture texture = xtr::load_texture("./data/textures/madoka.png");

    xtr::Array array;

    xtr::Buffer vertex_buffer{GL_ARRAY_BUFFER},
        element_buffer{GL_ELEMENT_ARRAY_BUFFER};

    array.bind();
    vertex_buffer.bind();
    vertex_buffer.data(cube.vertices.size() * sizeof(xtr::Vertex),
                       cube.vertices.data(), GL_STATIC_DRAW);

    element_buffer.bind();
    element_buffer.data(cube.indices.size() * sizeof(int), cube.indices.data(),
                        GL_STATIC_DRAW);

    xtr::vertex_attrib_mesh(0, 1, 2);

    vertex_buffer.unbind();

    array.unbind();

    bool mouse_left_down = false;

    while (app.is_running()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                app.quit();
            } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                       event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {
                mouse_left_down = true;
            } else if (event.type == SDL_MOUSEBUTTONUP &&
                       event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {
                mouse_left_down = false;
            } else if (event.type == SDL_MOUSEMOTION) {
                if (mouse_left_down) {
                    camera.set_phi(camera.get_phi() +
                                   float(event.motion.xrel) * 1e-2f);
                    camera.set_theta(camera.get_theta() +
                                     float(event.motion.yrel) * 1e-2f);
                }
            }
        }
        app.start_frame();

        ImGui::Begin("panel");
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, 800, 600);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();

        const glm::mat4 view_matrix = camera.view_matrix();
        const glm::mat4 projection_matrix =
            glm::perspective(glm::half_pi<float>(), 4.f / 3.f, 0.01f, 1e3f);

        glUniformMatrix4fv(program.loc("uni_model"), 1, GL_FALSE,
                           &model_matrix[0][0]);
        glUniformMatrix4fv(program.loc("uni_view"), 1, GL_FALSE,
                           &view_matrix[0][0]);
        glUniformMatrix4fv(program.loc("uni_projection"), 1, GL_FALSE,
                           &projection_matrix[0][0]);

        array.bind();
        glDrawElements(GL_TRIANGLES, GLuint(cube.indices.size()),
                       GL_UNSIGNED_INT, 0);

        app.end_frame();
    }
    return 0;
}
