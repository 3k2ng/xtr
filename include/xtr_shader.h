#pragma once
#include <filesystem>
#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <utility>

namespace xtr {
class Shader {
  public:
    static Shader from_file(const std::filesystem::path &file,
                            GLenum shader_type) {
        std::ifstream ifs(file);
        std::ostringstream oss;
        oss << ifs.rdbuf();
        Shader sh(shader_type);
        sh.source(oss.str().c_str());
        return sh;
    }

    Shader(GLenum shader_type) : _shader{glCreateShader(shader_type)} {}
    Shader(Shader &&o) : _shader{o._shader} {};
    Shader(const Shader &) = delete;
    Shader &operator=(Shader &&o) {
        _shader = o._shader;
        return *this;
    };
    Shader &operator=(const Shader &) = delete;
    ~Shader() { glDeleteShader(_shader); }

    inline void source(const char *shader_source) const {
        glShaderSource(_shader, 1, &shader_source, nullptr);
        glCompileShader(_shader);
    }

    inline void log_compile_status() const {
        GLint compile_successful;
        glGetShaderiv(_shader, GL_COMPILE_STATUS, &compile_successful);
        if (compile_successful == GL_TRUE) {
            std::cout << "Compile Successfully\n";
        } else {
            GLsizei log_length;
            GLchar message[1024];
            glGetShaderInfoLog(_shader, 1024, &log_length, message);
            std::cout << "Compile Failed\n" << message << "\n";
        }
    }

    inline operator GLuint() const { return _shader; }

  private:
    GLuint _shader;
};

class Program {
  public:
    Program() : _program{glCreateProgram()} {};
    Program(Program &&o) : _program{o._program} {};
    Program(const Program &) = delete;
    Program &operator=(Program &&o) {
        _program = o._program;
        return *this;
    };
    Program &operator=(const Program &) = delete;
    ~Program() { glDeleteProgram(_program); };

    inline void attach(const Shader &shader) const {
        glAttachShader(_program, shader);
    }

    inline void link() const { glLinkProgram(_program); }

    inline void log_link_status() const {
        GLint link_successful;
        glGetProgramiv(_program, GL_LINK_STATUS, &link_successful);
        if (link_successful == GL_TRUE) {
            std::cout << "Link Successfully\n";
        } else {
            GLsizei log_length;
            GLchar message[1024];
            glGetProgramInfoLog(_program, 1024, &log_length, message);
            std::cout << "Link Failed\n" << message << "\n";
        }
    }

    inline void use() const { glUseProgram(_program); }

    inline operator GLuint() const { return _program; }

  private:
    GLuint _program;
};
} // namespace xtr
