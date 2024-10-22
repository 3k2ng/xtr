#pragma once
#include <glm/glm.hpp>
#include <vector>
namespace xtr {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<size_t> indices;
};
} // namespace xtr
