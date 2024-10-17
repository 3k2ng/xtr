#pragma once
#include <SDL2/SDL.h>
#include <glad/gl.h>
namespace xtr {
class Texture {
  public:
    Texture(GLenum target) : _target{target} { glGenTextures(1, &_texture); }
    Texture(Texture &&o) : _texture{o._texture} {};
    Texture(const Texture &) = delete;
    Texture &operator=(Texture &&o) {
        _texture = o._texture;
        return *this;
    };
    Texture &operator=(const Texture &) = delete;
    ~Texture() { glDeleteTextures(1, &_texture); }

    inline void bind() const { glBindTexture(_target, _texture); }
    inline void unbind() const { glBindTexture(_target, 0); }

    inline const GLenum target() const { return _target; }
    inline operator GLuint() const { return _texture; }

  private:
    GLenum _target;
    GLuint _texture;
};
} // namespace xtr
