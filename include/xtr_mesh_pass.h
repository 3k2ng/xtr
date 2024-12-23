// standard procedure for loading and rendering a mesh into a framebuffer
// resulting framebuffer includes
// - position buffer
// - normal buffer
// - object id buffer

#pragma once
#include <xtr_buffer.h>
#include <xtr_framebuffer.h>
#include <xtr_mesh.h>
#include <xtr_shader.h>
#include <xtr_texture.h>
namespace xtr {
class MeshPass {
  public:
    MeshPass(const int width, const int height)
        : _program{load_program("./data/shaders/mesh.vert",
                                "./data/shaders/mesh.frag")},
          _array{}, _vertex_buffer{GL_ARRAY_BUFFER},
          _element_buffer{GL_ELEMENT_ARRAY_BUFFER},
          _position_texture{GL_TEXTURE_2D}, _normal_texture{GL_TEXTURE_2D},
          _id_texture{GL_TEXTURE_2D} {
        _array.bind();
        _vertex_buffer.bind();
        _element_buffer.bind();
        attrib_mesh(0, 1, 2);
        _array.unbind();
        resize(width, height);
        _framebuffer.bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, _position_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, _normal_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                               GL_TEXTURE_2D, _id_texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, _depth_buffer);
        unsigned int attachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
        };
        glDrawBuffers(3, attachments);
        _framebuffer.unbind();
    }

    // resize all of the buffers
    inline void resize(const int width, const int height) {
        _position_texture.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_FLOAT, nullptr);
        _position_texture.unbind();

        _normal_texture.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_FLOAT, nullptr);
        _normal_texture.unbind();

        _id_texture.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED,
                     GL_FLOAT, nullptr);
        _id_texture.unbind();

        _depth_buffer.bind();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
                              height);
        _depth_buffer.unbind();
    }

    // upload a mesh for drawing
    inline void upload_mesh(const xtr::Mesh &mesh) {
        _draw_count = mesh.indices.size();
        _array.bind();
        bind_mesh(mesh, _vertex_buffer, _element_buffer);
        _array.unbind();
    }

    // clear color and depth buffer
    inline void clear_buffer() const {
        _framebuffer.bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _framebuffer.unbind();
    }

    // render into buffer, with model-view-projection, normal_factor, and id
    inline void draw(const glm::mat4 &model_matrix,
                     const glm::mat4 &view_matrix,
                     const glm::mat4 &projection_matrix,
                     const float normal_factor, const int id) const {
        _framebuffer.bind();
        _program.use();
        _program.uni_mat4(_program.loc("uni_model"), model_matrix);
        _program.uni_mat4(_program.loc("uni_view"), view_matrix);
        _program.uni_mat4(_program.loc("uni_projection"), projection_matrix);
        _program.uni_1f(_program.loc("uni_normal_factor"), normal_factor);
        _program.uni_1i(_program.loc("uni_id"), id);
        _array.bind();
        glDrawElements(GL_TRIANGLES, _draw_count, GL_UNSIGNED_INT, 0);
        _array.unbind();
        _framebuffer.unbind();
    }

    inline void bind_framebuffer() const { _framebuffer.bind(); }
    inline void unbind_framebuffer() const { _framebuffer.unbind(); }

    // bind current buffers as textures
    inline void bind_buffers(const int uni_position, const int uni_normal,
                             const int uni_id) {
        if (uni_position >= 0) {
            glActiveTexture(GL_TEXTURE0 + uni_position);
            _position_texture.bind();
        }
        if (uni_normal >= 0) {
            glActiveTexture(GL_TEXTURE0 + uni_normal);
            _normal_texture.bind();
        }
        if (uni_id >= 0) {
            glActiveTexture(GL_TEXTURE0 + uni_id);
            _id_texture.bind();
        }
    };

    inline const xtr::Program &get_program() const { return _program; }

  private:
    xtr::Program _program;
    xtr::Array _array;
    xtr::Buffer _vertex_buffer, _element_buffer;
    xtr::Framebuffer _framebuffer;
    xtr::Texture _position_texture, _normal_texture, _id_texture;
    xtr::Renderbuffer _depth_buffer;
    GLsizei _draw_count;
};
} // namespace xtr
