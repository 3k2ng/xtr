// raii object for shader and program
#pragma once
#include <filesystem>
#include <fstream>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>

namespace xtr {
// shader object, load and compile shaders
class Shader {
  public:
    // load shader from file
    static Shader from_file(const std::filesystem::path &file_path,
                            GLenum shader_type) {
        std::ifstream ifs(file_path);
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

// program object, link program and set uniform values
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

    // uniform variable location from variable name
    inline GLint loc(const std::string &name) const {
        return glGetUniformLocation(_program, name.c_str());
    }

    // set uniform variable for some built-in types

    inline void uni_1f(const GLint loc, const GLfloat v0) const {
        glUniform1f(loc, v0);
    }

    inline void uni_2f(const GLint loc, const GLfloat v0,
                       const GLfloat v1) const {
        glUniform2f(loc, v0, v1);
    }

    inline void uni_3f(const GLint loc, const GLfloat v0, const GLfloat v1,
                       const GLfloat v2) const {
        glUniform3f(loc, v0, v1, v2);
    }

    inline void uni_4f(const GLint loc, const GLfloat v0, const GLfloat v1,
                       const GLfloat v2, const GLfloat v3) const {
        glUniform4i(loc, v0, v1, v2, v3);
    }

    inline void uni_1i(const GLint loc, const GLint v0) const {
        glUniform1i(loc, v0);
    }

    inline void uni_2i(const GLint loc, const GLint v0, const GLint v1) const {
        glUniform2i(loc, v0, v1);
    }

    inline void uni_3i(const GLint loc, const GLint v0, const GLint v1,
                       const GLint v2) const {
        glUniform3i(loc, v0, v1, v2);
    }

    inline void uni_4i(const GLint loc, const GLint v0, const GLint v1,
                       const GLint v2, const GLint v3) const {
        glUniform4i(loc, v0, v1, v2, v3);
    }

    inline void uni_vec2(const GLint loc, const glm::vec2 &v) const {
        glUniform2fv(loc, 1, (float *)&v);
    }

    inline void uni_vec3(const GLint loc, const glm::vec3 &v) const {
        glUniform3fv(loc, 1, (float *)&v);
    }

    inline void uni_vec4(const GLint loc, const glm::vec4 &v) const {
        glUniform4fv(loc, 1, (float *)&v);
    }

    inline void uni_mat2(const GLint loc, const glm::mat2 &v) const {
        glUniformMatrix2fv(loc, 1, GL_FALSE, (float *)&v);
    }

    inline void uni_mat3(const GLint loc, const glm::mat3 &v) const {
        glUniformMatrix3fv(loc, 1, GL_FALSE, (float *)&v);
    }

    inline void uni_mat4(const GLint loc, const glm::mat4 &v) const {
        glUniformMatrix4fv(loc, 1, GL_FALSE, (float *)&v);
    }

    inline operator GLuint() const { return _program; }

  private:
    GLuint _program;
};

// load standard vertex shader and fragment shader combo
inline Program load_program(const std::filesystem::path &vert,
                            const std::filesystem::path &frag) {
    xtr::Program program{};
    xtr::Shader vsh = xtr::Shader::from_file(vert, GL_VERTEX_SHADER);
    vsh.log_compile_status();
    xtr::Shader fsh = xtr::Shader::from_file(frag, GL_FRAGMENT_SHADER);
    fsh.log_compile_status();
    program.attach(vsh);
    program.attach(fsh);
    program.link();
    program.log_link_status();
    return program;
}
} // namespace xtr
