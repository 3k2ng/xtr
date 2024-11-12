#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
namespace xtr {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};
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
