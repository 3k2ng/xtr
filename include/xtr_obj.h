#pragma once
#include <filesystem>
#include <map>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <xtr_mesh.h>
namespace xtr {
inline Mesh load_obj_file(const std::filesystem::path &file_path) {
    tinyobj::ObjReader r;
    r.ParseFromFile(file_path);
    const auto &attrib = r.GetAttrib();
    const auto &shapes = r.GetShapes();
    std::vector<Vertex> vertices;
    std::vector<size_t> indices;
    auto oidcmp = [](const tinyobj::index_t &lhs, const tinyobj::index_t &rhs) {
        return lhs.vertex_index < rhs.vertex_index ||
               (lhs.vertex_index == rhs.vertex_index &&
                (lhs.normal_index < rhs.normal_index ||
                 (lhs.normal_index == rhs.normal_index &&
                  lhs.texcoord_index < rhs.texcoord_index)));
    };
    std::map<tinyobj::index_t, size_t, decltype(oidcmp)> oid2mid(
        oidcmp); // map obj index to mesh index
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                if (oid2mid.find(idx) == oid2mid.end()) {
                    tinyobj::real_t vx =
                        attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy =
                        attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz =
                        attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    glm::vec3 vn{0., 0., 0.};
                    if (idx.normal_index >= 0) {
                        vn.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vn.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vn.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }

                    glm::vec2 vt{0., 0.};
                    if (idx.texcoord_index >= 0) {
                        vt.x =
                            attrib
                                .texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vt.y =
                            attrib
                                .texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }
                    vertices.push_back({{vx, vy, vz}, vn, vt});
                    oid2mid[idx] = vertices.size();
                }
                indices.push_back(oid2mid[idx]);
            }
            index_offset += fv;
        }
    }
    return {vertices, indices};
}
} // namespace xtr
