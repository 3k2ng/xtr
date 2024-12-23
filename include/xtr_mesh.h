// a mesh used for the mesh pass, vertex include position, normal, and
// abstracted_normal
#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <xtr_buffer.h>
namespace xtr {

// vertex infomation
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 abstracted_normal;
};

// a mesh include a set of vertices and a set of indices
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};

// bind the current mesh to a VAO
inline void bind_mesh(const Mesh &mesh, const Buffer &vertex_buffer,
                      const Buffer &index_buffer) {
    vertex_buffer.bind();
    vertex_buffer.data(GLsizeiptr(mesh.vertices.size() * sizeof(Vertex)),
                       mesh.vertices.data(), GL_STATIC_DRAW);
    index_buffer.bind();
    index_buffer.data(GLsizeiptr(mesh.indices.size() * sizeof(int)),
                      mesh.indices.data(), GL_STATIC_DRAW);
}

// set vertex attribute for the current mesh
inline void attrib_mesh(int i_pos, int i_norm, int i_anorm) {
    if (i_pos >= 0) {
        glVertexAttribPointer(i_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, position));
        glEnableVertexAttribArray(i_pos);
    }
    if (i_norm >= 0) {
        glVertexAttribPointer(i_norm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(i_norm);
    }
    if (i_anorm >= 0) {
        glVertexAttribPointer(i_anorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, abstracted_normal));
        glEnableVertexAttribArray(i_anorm);
    }
}
} // namespace xtr
