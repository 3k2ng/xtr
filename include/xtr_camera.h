#pragma once
#include <glm/ext.hpp>
#include <imgui.h>

namespace xtr {
class TurnTableCamera {
  public:
    TurnTableCamera(float r, float theta, float phi, glm::vec3 const &origin)
        : _radial_distance{r}, _polar_angle{theta}, _azimuthal_angle{phi},
          _origin{origin} {}

    inline glm::mat4 view_matrix() const {
        return glm::lookAt(get_position(), _origin, {0., 1., 0.});
    }

    inline glm::vec3 get_position() const {
        return _origin + _radial_distance *
                             glm::vec3{
                                 sinf(_polar_angle) * cosf(_azimuthal_angle),
                                 cosf(_polar_angle),
                                 sinf(_polar_angle) * sinf(_azimuthal_angle),
                             };
    }

    inline glm::vec3 get_direction() const {
        return -1.f * glm::vec3{
                          sinf(_polar_angle) * cosf(_azimuthal_angle),
                          cosf(_polar_angle),
                          sinf(_polar_angle) * sinf(_azimuthal_angle),
                      };
    }

    inline float get_r() const { return _radial_distance; }
    inline void set_r(const float r) { _radial_distance = r; }

    inline float get_theta() const { return _polar_angle; }
    inline void set_theta(const float theta) { _polar_angle = theta; }

    inline float get_phi() const { return _azimuthal_angle; }
    inline void set_phi(const float phi) { _azimuthal_angle = phi; }

    inline glm::vec3 get_origin() const { return _origin; }
    inline void set_origin(const glm::vec3 origin) { _origin = origin; }

    void update_origin(const glm::vec3 delta) {
        _origin += delta * _move_speed;
    }

    inline void imgui() {
        if (ImGui::TreeNode("Camera")) {
            ImGui::InputFloat3("origin", (float *)&_origin);
            ImGui::InputFloat("r", &_radial_distance, 1.f);
            ImGui::InputFloat("theta", &_polar_angle, 1e-2f);
            ImGui::InputFloat("phi", &_azimuthal_angle, 1e-2f);
            ImGui::TreePop();
        }
    }

  private:
    float _radial_distance;
    float _polar_angle;
    float _azimuthal_angle;
    glm::vec3 _origin;
    float _move_speed = 5.f;
};
} // namespace xtr
