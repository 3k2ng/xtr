// raii object for the framebuffer and the renderbuffer
// mainly use the manage lifetime and binding
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
class Renderbuffer {
  public:
    static void unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

    Renderbuffer() { glGenRenderbuffers(1, &_renderbuffer); }
    Renderbuffer(Renderbuffer &&o) : _renderbuffer{o._renderbuffer} {};
    Renderbuffer(const Renderbuffer &) = delete;
    Renderbuffer &operator=(Renderbuffer &&o) {
        _renderbuffer = o._renderbuffer;
        return *this;
    };
    Renderbuffer &operator=(const Renderbuffer &) = delete;
    ~Renderbuffer() { glDeleteRenderbuffers(1, &_renderbuffer); }

    inline void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    }

    inline operator GLuint() const { return _renderbuffer; }

  private:
    GLuint _renderbuffer;
};
} // namespace xtr
