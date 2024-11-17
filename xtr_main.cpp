#include "SDL_keycode.h"
#include "imgui.h"
#include <xtr_app.h>
#include <xtr_buffer.h>
#include <xtr_camera.h>
#include <xtr_framebuffer.h>
#include <xtr_mesh_pass.h>
#include <xtr_obj.h>
#include <xtr_screen_pass.h>
#include <xtr_shader.h>
#include <xtr_texture.h>

constexpr int mini(const int x, const int y) { return x < y ? x : y; }
constexpr int maxi(const int x, const int y) { return x > y ? x : y; }

int main(int argc, char *argv[]) {
    xtr::App app{800, 600};
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    xtr::ScreenPass screen_pass{"./data/shaders/screen_loa.frag"};
    xtr::MeshPass mesh_pass{"./data/shaders/buffer_pass.frag"};
    xtr::TurnTableCamera camera{1000.f, 11.f / 24.f * glm::pi<float>(), 0., {}};
    glm::mat4 model_matrix{1.};
    glm::mat4 projection_matrix =
        glm::perspective(glm::half_pi<float>(), 4.f / 3.f, 1e-3f, 1e4f);

    if (argc > 1) {
        mesh_pass.upload_mesh(xtr::load_mesh(std::string("./data/models/")+argv[1]+".ply", true));
    } else {
        mesh_pass.upload_mesh(xtr::load_mesh("./data/models/Venus.ply", true));
    }

    xtr::Framebuffer framebuffer;

    xtr::Texture z_buffer_texture{GL_TEXTURE_2D};
    z_buffer_texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app.get_screen_width(),
                 app.get_screen_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    z_buffer_texture.unbind();

    xtr::Texture obam_texture{GL_TEXTURE_2D};
    obam_texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app.get_screen_width(),
                 app.get_screen_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    obam_texture.unbind();

    xtr::Texture tonemap_texture =
        xtr::load_texture("./data/textures/fig-9e.ppm");

    xtr::Renderbuffer renderbuffer;
    renderbuffer.bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          app.get_screen_width(), app.get_screen_height());
    renderbuffer.unbind();

    framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           z_buffer_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           obam_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, renderbuffer);
    unsigned int attachments[2] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
    };
    glDrawBuffers(2, attachments);
    framebuffer.unbind();

    char imgui_mesh_name[50] = {"Venus"};

    app.enable_imgui = true;
    while (app.is_running()) {
        app.update_input();
        if (app.is_button_down(SDL_BUTTON_LEFT)) {
            camera.set_phi(camera.get_phi() + (app.get_mouse_delta().x * 1e1f));
            camera.set_theta(camera.get_theta() +
                             (app.get_mouse_delta().y * 1e1f));
        }
        camera.set_r(camera.get_r() - (app.get_wheel_delta().y * 1e1f));
        const glm::vec3 origin_delta = {
            app.is_key_down(SDLK_RIGHT) - app.is_key_down(SDLK_LEFT),
            app.is_key_down(SDLK_UP) - app.is_key_down(SDLK_DOWN),
            app.is_key_down(SDLK_PERIOD) - app.is_key_down(SDLK_COMMA)
        };
        camera.update_origin(origin_delta);

        app.start_frame();
        if (app.enable_imgui) {
            ImGui::Begin("panel");
            camera.imgui();
            ImGui::Separator();
            ImGui::InputTextWithHint("Mesh Name", "Venus", imgui_mesh_name, 50);
            if (ImGui::Button("Load Mesh")) {
                mesh_pass.upload_mesh(xtr::load_mesh(std::string("./data/models/")+imgui_mesh_name+".ply", true));
            }
            ImGui::End();
            ImGui::Render();
        }

        glViewport(0, 0, app.get_screen_width(), app.get_screen_height());
        framebuffer.bind();
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const xtr::Program &mesh_pass_program = mesh_pass.get_program();
        mesh_pass_program.use();
        mesh_pass_program.uni_1i(mesh_pass_program.loc("uni_dof"), false);
        mesh_pass_program.uni_vec3(mesh_pass_program.loc("uni_camera_pos"),
                                   camera.get_position());
        mesh_pass_program.uni_vec3(mesh_pass_program.loc("uni_camera_dir"),
                                   camera.get_direction());
        mesh_pass.draw(model_matrix, camera.view_matrix(), projection_matrix);

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        std::vector<glm::vec4> z_buffer(app.get_screen_width() *
                                        app.get_screen_height());
        glReadPixels(0, 0, app.get_screen_width(), app.get_screen_height(),
                     GL_RGBA, GL_FLOAT, z_buffer.data());
        framebuffer.unbind();

        float z_max = 0.f, z_min = 1e32f;
        z_max = 1e4f, z_min = 1e2f;
        // for (const auto &pixel : z_buffer) {
        //     if (pixel.x > 0) {
        //         z_max = std::max(pixel.x, z_max);
        //         z_min = std::min(pixel.x, z_min);
        //     }
        // }

        glActiveTexture(GL_TEXTURE0);
        z_buffer_texture.bind();
        glActiveTexture(GL_TEXTURE1);
        obam_texture.bind();
        glActiveTexture(GL_TEXTURE2);
        tonemap_texture.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const xtr::Program &screen_pass_program = screen_pass.get_program();
        screen_pass_program.use();
        screen_pass_program.uni_1f(screen_pass_program.loc("uni_z_min"), z_min);
        screen_pass_program.uni_1f(screen_pass_program.loc("uni_z_max"), z_max);

        screen_pass_program.uni_1i(screen_pass_program.loc("uni_z_buffer"), 0);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_obam"), 1);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_tonemap"), 2);
        screen_pass.draw();

        app.end_frame();
    }
    return 0;
}
