#pragma once
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <glad/gl.h>

namespace xtr {
class Texture {
  public:
    Texture(GLenum target, const bool is_repeat = false,
            const bool is_linear = false)
        : _target{target} {
        glGenTextures(1, &_texture);
        bind();
        glTexParameteri(_target, GL_TEXTURE_WRAP_S,
                        is_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T,
                        is_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER,
                        is_linear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER,
                        is_linear ? GL_LINEAR : GL_NEAREST);
        unbind();
    }
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

    inline void load_surface(const SDL_Surface &surface,
                             const bool is_repeat = false,
                             const bool is_linear = false) {
        bind();
        SDL_Surface *rgba_surface = SDL_ConvertSurfaceFormat(
            (SDL_Surface *)(&surface), SDL_PIXELFORMAT_RGBA32, 0);
        glTexImage2D(_target, 0, GL_RGBA, rgba_surface->w, rgba_surface->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, rgba_surface->pixels);
        unbind();
    }

    inline void load_file(const std::filesystem::path &file_path,
                          const bool is_repeat = false,
                          const bool is_linear = false) {
        SDL_Surface *surface = IMG_Load(file_path.c_str());
        load_surface(*surface, is_repeat, is_linear);
        SDL_FreeSurface(surface);
    }

  private:
    GLenum _target;
    GLuint _texture;
};
} // namespace xtr
