#pragma once
#include <xtr_buffer.h>
#include <xtr_mesh.h>
#include <xtr_shader.h>
namespace xtr {
class MeshPass {
  public:
    MeshPass(const std::filesystem::path &frag)
        : _program{load_program("./data/shaders/mesh.vert", frag)}, _array{},
          _vertex_buffer{GL_ARRAY_BUFFER},
          _element_buffer{GL_ELEMENT_ARRAY_BUFFER} {
        _array.bind();
        _vertex_buffer.bind();
        _element_buffer.bind();
        attrib_mesh(0, 1);
        _array.unbind();
    }

    inline void upload_mesh(const xtr::Mesh &mesh) {
        _draw_count = mesh.indices.size();
        _array.bind();
        bind_mesh(mesh, _vertex_buffer, _element_buffer);
        _array.unbind();
    }

    inline void draw(const glm::mat4 &model_matrix,
                     const glm::mat4 &view_matrix,
                     const glm::mat4 &projection_matrix,
                     const float norm_fac) const {
        _program.use();
        _program.uni_mat4(_program.loc("uni_model"), model_matrix);
        _program.uni_mat4(_program.loc("uni_view"), view_matrix);
        _program.uni_mat4(_program.loc("uni_projection"), projection_matrix);
        _program.uni_1f(_program.loc("uni_norm_fac"), norm_fac);
        _array.bind();
        glDrawElements(GL_TRIANGLES, _draw_count, GL_UNSIGNED_INT, 0);
        _array.unbind();
    }

    inline const xtr::Program &get_program() const { return _program; }

  private:
    xtr::Program _program;
    xtr::Array _array;
    xtr::Buffer _vertex_buffer, _element_buffer;
    GLsizei _draw_count;
};
} // namespace xtr
