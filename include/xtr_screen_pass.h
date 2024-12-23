// screen rendering procedure
// used to combine framebuffer data, or perform post-processing on an image
// the fragment shader is customized
#pragma once
#include <xtr_buffer.h>
#include <xtr_shader.h>
namespace xtr {
static const int indices[] = {0, 1, 2};
class ScreenPass {
  public:
    ScreenPass(const std::filesystem::path &frag)
        : _program{load_program("./data/shaders/screen.vert", frag)}, _array{},
          _element_buffer{GL_ELEMENT_ARRAY_BUFFER} {
        _array.bind();
        _element_buffer.bind();
        _element_buffer.data(sizeof(indices), indices, GL_STATIC_DRAW);
        _array.unbind();
    }

    // simply draw a big triangle that cover the whole screen
    // more on screen.vert
    inline void draw() const {
        _program.use();
        _array.bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        _array.unbind();
    }

    inline const xtr::Program &get_program() const { return _program; }

  private:
    xtr::Program _program;
    xtr::Array _array;
    xtr::Buffer _element_buffer;
};
} // namespace xtr
