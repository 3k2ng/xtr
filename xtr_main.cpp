#include <xtr_app.h>
#include <xtr_buffer.h>
#include <xtr_camera.h>
#include <xtr_framebuffer.h>
#include <xtr_mesh_pass.h>
#include <xtr_obj.h>
#include <xtr_screen_pass.h>
#include <xtr_shader.h>
#include <xtr_texture.h>

int main(int argc, char *argv[]) {
    xtr::App app{};
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    xtr::ScreenPass screen_pass{"./data/shaders/screen.frag"};
    xtr::MeshPass mesh_pass{"./data/shaders/basic.frag"};
    xtr::TurnTableCamera camera{1000.f, glm::half_pi<float>(), 0., {}};
    glm::mat4 model_matrix{1.};
    glm::mat4 projection_matrix =
        glm::perspective(glm::half_pi<float>(), 4.f / 3.f, 1e-3f, 1e4f);

    xtr::Texture texture = xtr::load_texture("./data/textures/fig-7b.ppm");

    mesh_pass.upload_mesh(xtr::load_mesh("./data/models/Venus.ply"));

    app.enable_imgui = false;
    while (app.is_running()) {
        app.update_input();
        if (app.is_button_down(SDL_BUTTON_LEFT)) {
            camera.set_phi(camera.get_phi() + app.get_mouse_delta().x * 1e2f);
            camera.set_theta(camera.get_theta() +
                             app.get_mouse_delta().y * 1e2f);
        }
        camera.set_r(camera.get_r() - app.get_wheel_delta().y * 1e2f);

        app.start_frame();
        if (app.enable_imgui) {
            ImGui::Begin("panel");
            camera.imgui();
            ImGui::End();
            ImGui::Render();
        }

        glViewport(0, 0, app.get_screen_width(), app.get_screen_height());
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh_pass.draw(model_matrix, camera.view_matrix(), projection_matrix);

        app.end_frame();
    }
    return 0;
}
