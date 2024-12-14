#include <imgui.h>
#include <optional>
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
    xtr::MeshPass mesh_pass;
    xtr::TurnTableCamera camera{1.f, 11.f / 24.f * glm::pi<float>(), 0., {}};
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

    xtr::Framebuffer framebuffer;

    xtr::Texture position_texture{GL_TEXTURE_2D};
    position_texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, app.get_screen_width(),
                 app.get_screen_height(), 0, GL_RGB, GL_FLOAT, nullptr);
    position_texture.unbind();

    xtr::Texture normal_texture{GL_TEXTURE_2D};
    normal_texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, app.get_screen_width(),
                 app.get_screen_height(), 0, GL_RGB, GL_FLOAT, nullptr);
    normal_texture.unbind();

    xtr::Texture id_texture{GL_TEXTURE_2D};
    id_texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, app.get_screen_width(),
                 app.get_screen_height(), 0, GL_RED, GL_INT, nullptr);
    id_texture.unbind();

    xtr::Texture tonemap_texture{GL_TEXTURE_2D};
    tonemap_texture.load_file(texture_files[0]);

    xtr::Renderbuffer renderbuffer;
    renderbuffer.bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          app.get_screen_width(), app.get_screen_height());
    renderbuffer.unbind();

    framebuffer.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           position_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           normal_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           id_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, renderbuffer);
    unsigned int attachments[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    glDrawBuffers(3, attachments);
    framebuffer.unbind();

    int selected_mesh = 0;
    bool mesh_y_up = true;
    bool mesh_x_front = true;
    int selected_texture = 0;

    const char *detail_mappings[] = {"LOA", "Depth-of-field", "Near-silhouette",
                                     "Specular highlights"};
    int detail_mapping = 0;

    // Depth-based attribute mapping
    float dbam_z_min = 0.5f;
    float dbam_r = 5.f;
    glm::vec3 dof_c = {};

    // Near-silhouette
    float near_silhouette_r = 0.;

    // Specular highlights
    float specular_s = 1.;

    const char *abstracted_shapes[] = {"Smooth", "Ellipse", "Cylinder",
                                       "Sphere"};
    int abstracted_shape = 0;
    float normal_factor = 0.;

    float light_theta, light_phi;

    app.enable_imgui = true;
    while (app.is_running()) {
        if (app.is_window_resized()) {
            position_texture.bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, app.get_screen_width(),
                         app.get_screen_height(), 0, GL_RGB, GL_FLOAT, nullptr);
            position_texture.unbind();

            normal_texture.bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, app.get_screen_width(),
                         app.get_screen_height(), 0, GL_RGB, GL_FLOAT, nullptr);
            normal_texture.unbind();

            id_texture.bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, app.get_screen_width(),
                         app.get_screen_height(), 0, GL_RED, GL_FLOAT, nullptr);
            id_texture.unbind();

            renderbuffer.bind();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                  app.get_screen_width(),
                                  app.get_screen_height());
            renderbuffer.unbind();

            glViewport(0, 0, app.get_screen_width(), app.get_screen_height());
        }

        projection_matrix =
            glm::perspective(glm::half_pi<float>(),
                             static_cast<float>(app.get_screen_width()) /
                                 static_cast<float>(app.get_screen_height()),
                             1e-3f, 1e4f);
        if (app.is_button_down(SDL_BUTTON_LEFT)) {
            camera.set_phi(camera.get_phi() + (app.get_mouse_delta().x * 30.f));
            camera.set_theta(camera.get_theta() +
                             (app.get_mouse_delta().y * 30.f));
        }
        std::optional<glm::vec2> c_pick = std::nullopt;
        if (app.is_button_down(SDL_BUTTON_RIGHT)) {
            c_pick = app.get_mouse_position();
        }
        camera.set_r(camera.get_r() - (app.get_wheel_delta().y * 0.03f));
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
            if (ImGui::Checkbox("y_up", &mesh_y_up) ||
                ImGui::Checkbox("x_front", &mesh_x_front)) {
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

            ImGui::Combo("Detail Mapping", &detail_mapping, detail_mappings, 4);
            if (detail_mapping < 2) {
                ImGui::DragFloat("z_min", &dbam_z_min, 0.1f, 0.1f, 10.f);
                ImGui::DragFloat("r", &dbam_r, 1e-2f, 1. + 1e-3f, 100.f);
            }
            if (detail_mapping == 0) {        // LOA
            } else if (detail_mapping == 1) { // Depth-of-field
                ImGui::DragFloat3("c", &dof_c.x);
            } else if (detail_mapping == 2) { // Near-silhouette
                ImGui::DragFloat("Magnitude", &near_silhouette_r, 1e-2f, 1e-3f,
                                 1e4f);
            } else if (detail_mapping == 3) { // Specular highlights
                ImGui::DragFloat("Shininess", &specular_s, 1e-2f, 1e-3f, 1e4f);
            }

            ImGui::Separator();
            ImGui::DragFloat("Normal Abstraction", &normal_factor, 1e-2f, 0.f,
                             1.f);
            if (ImGui::Combo("Abstracted Shape", &abstracted_shape,
                             abstracted_shapes, 4)) {
                mesh_pass.upload_mesh(xtr::load_mesh(mesh_files[selected_mesh],
                                                     abstracted_shape,
                                                     mesh_y_up, mesh_x_front));
            }
            ImGui::Separator();
            ImGui::Text("Light Direction");
            ImGui::DragFloat("theta", &light_theta, 1e-2f);
            ImGui::DragFloat("phi", &light_phi, 1e-2f);
            ImGui::End();
            ImGui::Render();
        }
        framebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh_pass.draw(model_matrix, camera.view_matrix(), projection_matrix,
                       normal_factor, 69);

        glReadBuffer(GL_COLOR_ATTACHMENT0);
        std::vector<glm::vec3> position_buffer(app.get_screen_width() *
                                               app.get_screen_height());
        glReadPixels(0, 0, app.get_screen_width(), app.get_screen_height(),
                     GL_RGB, GL_FLOAT, position_buffer.data());

        framebuffer.unbind();

        if (c_pick.has_value()) {
            int x = c_pick.value().x * app.get_screen_width();
            int y = (1. - c_pick.value().y) * app.get_screen_height();
            dof_c = position_buffer[x + y * app.get_screen_width()];
        }

        glActiveTexture(GL_TEXTURE0);
        position_texture.bind();
        glActiveTexture(GL_TEXTURE1);
        normal_texture.bind();
        glActiveTexture(GL_TEXTURE2);
        id_texture.bind();
        glActiveTexture(GL_TEXTURE3);
        tonemap_texture.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const xtr::Program &screen_pass_program = screen_pass.get_program();
        screen_pass_program.use();

        screen_pass_program.uni_1i(screen_pass_program.loc("uni_position"), 0);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_normal"), 1);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_id_map"), 2);
        screen_pass_program.uni_1i(screen_pass_program.loc("uni_tonemap"), 3);

        screen_pass_program.uni_1i(screen_pass_program.loc("uni_id"), 69);

        screen_pass_program.uni_vec3(screen_pass_program.loc("uni_camera_pos"),
                                     camera.get_position());
        screen_pass_program.uni_vec3(screen_pass_program.loc("uni_camera_dir"),
                                     camera.get_direction());

        screen_pass_program.uni_1i(
            screen_pass_program.loc("uni_detail_mapping"), detail_mapping);

        screen_pass_program.uni_1f(
            screen_pass_program.loc("uni_near_silhouette_r"),
            near_silhouette_r);
        screen_pass_program.uni_1f(screen_pass_program.loc("uni_specular_s"),
                                   specular_s);

        screen_pass_program.uni_1f(screen_pass_program.loc("uni_dbam_z_min"),
                                   dbam_z_min);
        screen_pass_program.uni_1f(screen_pass_program.loc("uni_dbam_r"),
                                   dbam_r);
        screen_pass_program.uni_1f(screen_pass_program.loc("uni_dof_z_c"),
                                   glm::length(dof_c - camera.get_position()));

        screen_pass_program.uni_vec3(screen_pass_program.loc("uni_light_dir"),
                                     glm::vec3{
                                         sinf(light_theta) * cosf(light_phi),
                                         cosf(light_theta),
                                         sinf(light_theta) * sinf(light_phi),
                                     });

        screen_pass.draw();
        app.end_frame();
    }
    return 0;
}
