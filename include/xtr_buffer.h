// raii object for VBO and VAO
#pragma once
#include <glad/gl.h>

namespace xtr {
// short for vertex buffer object
class Buffer {
  public:
    static void unbind(GLenum target) { glBindBuffer(target, 0); }

    Buffer(GLenum target) : _target{target} { glGenBuffers(1, &_buffer); }
    Buffer(Buffer &&o) : _target{o._target}, _buffer{o._buffer} {};
    Buffer(const Buffer &) = delete;
    Buffer &operator=(Buffer &&o) {
        _target = o._target;
        _buffer = o._buffer;
        return *this;
    };
    Buffer &operator=(const Buffer &) = delete;
    ~Buffer() { glDeleteBuffers(1, &_buffer); }

    inline void data(GLsizeiptr size, const GLvoid *data, GLenum usage) const {
        glBufferData(_target, size, data, usage);
    }

    inline void bind() const { glBindBuffer(_target, _buffer); }
    inline void unbind() const { glBindBuffer(_target, 0); }

    inline const GLenum target() const { return _target; }
    inline operator GLuint() const { return _buffer; }

  private:
    GLenum _target;
    GLuint _buffer;
};

// short for vertex array object
class Array {
  public:
    static void unbind() { glBindVertexArray(0); }

    Array() { glGenVertexArrays(1, &_array); }
    Array(Array &&o) : _array{o._array} {};
    Array(const Array &) = delete;
    Array &operator=(Array &&o) {
        _array = o._array;
        return *this;
    };
    Array &operator=(const Array &) = delete;
    ~Array() { glDeleteVertexArrays(1, &_array); }

    inline void bind() const { glBindVertexArray(_array); }

    inline operator GLuint() const { return _array; }

  private:
    GLuint _array;
};
}; // namespace xtr
