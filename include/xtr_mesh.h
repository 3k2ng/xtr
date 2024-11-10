#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
namespace xtr {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};
inline void vertex_attrib_mesh(int i_pos, int i_norm, int i_tc) {
    if (i_pos >= 0) {
        glVertexAttribPointer(i_pos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void *)offsetof(xtr::Vertex, position));
        glEnableVertexAttribArray(i_pos);
    }
    if (i_norm >= 0) {
        glVertexAttribPointer(i_norm, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void *)offsetof(xtr::Vertex, normal));
        glEnableVertexAttribArray(i_norm);
    }
    if (i_tc >= 0) {
        glVertexAttribPointer(i_tc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void *)offsetof(xtr::Vertex, texcoord));
        glEnableVertexAttribArray(i_tc);
    }
}
} // namespace xtr
