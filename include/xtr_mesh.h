#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <xtr_buffer.h>
namespace xtr {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};
inline void bind_mesh(const Mesh &mesh, const Buffer &vertex_buffer,
                      const Buffer &index_buffer) {
    vertex_buffer.bind();
    vertex_buffer.data(GLsizeiptr(mesh.vertices.size() * sizeof(Vertex)),
                       mesh.vertices.data(), GL_STATIC_DRAW);
    index_buffer.bind();
    index_buffer.data(GLsizeiptr(mesh.indices.size() * sizeof(int)),
                      mesh.indices.data(), GL_STATIC_DRAW);
}
inline void attrib_mesh(int i_pos, int i_norm) {
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
}
} // namespace xtr
