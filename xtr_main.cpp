#include <imgui.h>
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
    xtr::ScreenPass screen_pass{"./data/shaders/screen_xtoon.frag"};
    xtr::MeshPass mesh_pass{"./data/shaders/buffer_pass.frag"};
    xtr::TurnTableCamera camera{1000.f, 11.f / 24.f * glm::pi<float>(), 0., {}};
    glm::mat4 model_matrix{1.};
    glm::mat4 projection_matrix =
        glm::perspective(glm::half_pi<float>(), 4.f / 3.f, 1e-3f, 1e4f);

    const std::filesystem::path mesh_directory = "./data/models";
    std::vector<std::filesystem::path> mesh_files;
    for (const auto &file :
         std::filesystem::directory_iterator{mesh_directory}) {
        mesh_files.push_back(file);
    }

    mesh_pass.upload_mesh(xtr::load_mesh(mesh_files[0], 0, true, true));

    const std::filesystem::path texture_directory = "./data/textures";
    std::vector<std::filesystem::path> texture_files;
    for (const auto &file :
         std::filesystem::directory_iterator{texture_directory}) {
        texture_files.push_back(file);
    }

    const char *abstracted_shapes[] = {"Smooth", "Ellipse", "Cylinder",
                                       "Sphere"};

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

    xtr::Texture tonemap_texture{GL_TEXTURE_2D};
    tonemap_texture.load_file(texture_files[0]);

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

    int selected_mesh = 0;
    bool mesh_y_up = true;
    bool mesh_x_front = true;
    int selected_texture = 0;

    float z_min = 100.f;
    float r = 2.f;
    float obam_r = 1.f;
    float obam_s = 10.f;
    float norm_fac = 0.f;
    glm::vec2 z_c_pick = glm::vec2(.5f, .5f);
    bool use_dof = false;
    bool use_obam = false;
    bool use_sbam = false;

    int abstracted_shape = 0;

    app.enable_imgui = true;
    while (app.is_running()) {
        app.update_input();
        if (app.is_button_down(SDL_BUTTON_LEFT)) {
            camera.set_phi(camera.get_phi() + (app.get_mouse_delta().x * 1e1f));
            camera.set_theta(camera.get_theta() +
                             (app.get_mouse_delta().y * 1e1f));
        }
        if (app.is_button_down(SDL_BUTTON_RIGHT)) {
            z_c_pick = app.get_mouse_position();
        }
        camera.set_r(camera.get_r() - (app.get_wheel_delta().y * 1e1f));
        const glm::vec3 origin_delta = {
            app.is_key_down(SDLK_RIGHT) - app.is_key_down(SDLK_LEFT),
            app.is_key_down(SDLK_UP) - app.is_key_down(SDLK_DOWN),
            app.is_key_down(SDLK_PERIOD) - app.is_key_down(SDLK_COMMA)};
        camera.update_origin(origin_delta);

        app.start_frame();
        if (app.enable_imgui) {
            ImGui::Begin("panel");
            camera.imgui();
            ImGui::Separator();
            if (ImGui::BeginCombo(
                    "Mesh", mesh_files[selected_mesh].filename().c_str())) {
                for (int i = 0; i < mesh_files.size(); ++i) {
                    const bool is_selected = selected_mesh == i;
                    if (ImGui::Selectable(mesh_files[i].filename().c_str(),
                                          is_selected)) {
                        selected_mesh = i;
                        mesh_pass.upload_mesh(
                            xtr::load_mesh(mesh_files[i], abstracted_shape,
                                           mesh_y_up, mesh_x_front));
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::Checkbox("mesh_y_up", &mesh_y_up) ||
                ImGui::Checkbox("mesh_x_front", &mesh_x_front)) {
                mesh_pass.upload_mesh(xtr::load_mesh(mesh_files[selected_mesh],
                                                     abstracted_shape,
                                                     mesh_y_up, mesh_x_front));
            }
            ImGui::Separator();

            if (ImGui::BeginCombo(
                    "Texture",
                    texture_files[selected_texture].filename().c_str())) {
                for (int i = 0; i < texture_files.size(); ++i) {
                    const bool is_selected = selected_texture == i;
                    if (ImGui::Selectable(texture_files[i].filename().c_str(),
                                          is_selected)) {
                        selected_texture = i;
                        tonemap_texture.load_file(texture_files[i]);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::DragFloat("z_min", &z_min, 5.f, 5.f, 1e8f);
            ImGui::DragFloat("r", &r, 0.05f, 1.05f, 1e8f);
            ImGui::LabelText("z_c_pick", "%f, %f", z_c_pick.x, z_c_pick.y);
            ImGui::Checkbox("Use Depth of Field", &use_dof);
            ImGui::Separator();
            ImGui::Checkbox("Use Near-Silhouette (OBAM)", &use_obam);
            ImGui::DragFloat("Near-Silhouette Magnitude", &obam_r, 0.1f, 0.1f,
                             1e8f);
            ImGui::Checkbox("Use Specular Based (OBAM)", &use_sbam);
            ImGui::DragFloat("Specular Shininess", &obam_s, 0.1f, 1.1f, 1e8f);
            ImGui::Separator();
            ImGui::DragFloat("Normal Abstraction", &norm_fac, 0.01f, 0.f, 1.f);
            if (ImGui::Combo("Abstracted Shape", &abstracted_shape,
                             abstracted_shapes, 4)) {
                mesh_pass.upload_mesh(xtr::load_mesh(mesh_files[selected_mesh],
                                                     abstracted_shape,
                                                     mesh_y_up, mesh_x_front));
            }
            ImGui::End();
            ImGui::Render();
        }

        glViewport(0, 0, app.get_screen_width(), app.get_screen_height());
        framebuffer.bind();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const xtr::Program &mesh_pass_program = mesh_pass.get_program();
        mesh_pass_program.use();
        mesh_pass_program.uni_vec3(mesh_pass_program.loc("uni_camera_pos"),
                                   camera.get_position());
        mesh_pass_program.uni_vec3(mesh_pass_program.loc("uni_camera_dir"),
                                   camera.get_direction());
        mesh_pass_program.uni_1i(mesh_pass_program.loc("uni_use_obam"),
                                 use_obam);
        mesh_pass_program.uni_1i(mesh_pass_program.loc("uni_use_sbam"),
                                 use_sbam);
        mesh_pass_program.uni_1f(mesh_pass_program.loc("uni_r"), obam_r);
        mesh_pass_program.uni_1f(mesh_pass_program.loc("uni_s"), obam_s);
        mesh_pass.draw(model_matrix, camera.view_matrix(), projection_matrix,
                       norm_fac);

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        std::vector<glm::vec4> z_buffer(app.get_screen_width() *
                                        app.get_screen_height());
        glReadPixels(0, 0, app.get_screen_width(), app.get_screen_height(),
                     GL_RGBA, GL_FLOAT, z_buffer.data());
        framebuffer.unbind();

        float z_max;
        if (use_obam || use_sbam) {
            z_max = 0.f;
            for (const auto &pixel : z_buffer) {
                if (pixel.x > 0) {
                    z_max = std::max(pixel.x, z_max);
                }
            }
        } else {
            z_max = z_min * r;
        }

        float z_c = z_buffer[app.get_screen_width() * z_c_pick.x +
                             (app.get_screen_height() * z_c_pick.y) *
                                 app.get_screen_width()]
                        .x;

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

        screen_pass_program.uni_1f(screen_pass_program.loc("uni_z_c"), z_c);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_use_dof"),
                                   use_dof);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_use_obam"),
                                   use_obam);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_use_sbam"),
                                   use_sbam);
        screen_pass.draw();

        app.end_frame();
    }
    return 0;
}
