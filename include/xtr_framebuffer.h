#pragma once
#include <xtr_shader.h>

namespace xtr {
class Framebuffer {
  public:
    static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    Framebuffer() { glGenFramebuffers(1, &_framebuffer); }
    Framebuffer(Framebuffer &&o) : _framebuffer{o._framebuffer} {};
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(Framebuffer &&o) {
        _framebuffer = o._framebuffer;
        return *this;
    };
    Framebuffer &operator=(const Framebuffer &) = delete;
    ~Framebuffer() { glDeleteFramebuffers(1, &_framebuffer); }

    inline void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    }

    inline operator GLuint() const { return _framebuffer; }

  private:
    GLuint _framebuffer;
};
} // namespace xtr
