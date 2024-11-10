#pragma once
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
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

inline Texture load_texture_from_surface(const SDL_Surface &surface,
                                         const bool is_repeat = false,
                                         const bool is_linear = false) {
    Texture texture{GL_TEXTURE_2D};
    glBindTexture(texture.target(), texture);
    glTexParameteri(texture.target(), GL_TEXTURE_WRAP_S,
                    is_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(texture.target(), GL_TEXTURE_WRAP_T,
                    is_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(texture.target(), GL_TEXTURE_MIN_FILTER,
                    is_linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(texture.target(), GL_TEXTURE_MAG_FILTER,
                    is_linear ? GL_LINEAR : GL_NEAREST);
    glTexImage2D(texture.target(), 0, GL_RGBA, surface.w, surface.h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface.pixels);
    return texture;
}

inline Texture load_texture(const std::filesystem::path &file,
                            const bool is_repeat = false,
                            const bool is_linear = false) {
    SDL_Surface *surface = IMG_Load(file.c_str());
    Texture texture = load_texture_from_surface(*surface, is_repeat, is_linear);
    SDL_FreeSurface(surface);
    return texture;
}
} // namespace xtr
