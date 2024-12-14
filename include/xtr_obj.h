#pragma once
#include <filesystem>
#include <map>
#include <miniply.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <xtr_mesh.h>

namespace xtr {
inline std::pair<std::vector<glm::vec3>, std::vector<int>>
load_obj_file(const std::filesystem::path &file_path) {
    tinyobj::ObjReader r;
    r.ParseFromFile(file_path);
    const auto &attrib = r.GetAttrib();
    const auto &shapes = r.GetShapes();
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;

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
                    oid2mid[idx] = vertices.size();
                    vertices.push_back({vx, vy, vz});
                }
                indices.push_back(oid2mid[idx]);
            }
            index_offset += fv;
        }
    }
    return {vertices, indices};
}

inline std::pair<std::vector<glm::vec3>, std::vector<int>>
load_ply_file(const std::filesystem::path &file_path) {
    miniply::PLYReader r(file_path.c_str());
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;
    unsigned int element_indices[3];
    bool vertices_loaded = false, faces_loaded = false;
    while (r.has_element() && !(vertices_loaded && faces_loaded)) {
        if (r.element_is(miniply::kPLYVertexElement) && r.load_element() &&
            r.find_pos(element_indices)) {
            vertices.resize(r.num_rows());
            r.extract_properties(element_indices, 3,
                                 miniply::PLYPropertyType::Float,
                                 vertices.data());
            vertices_loaded = true;
        } else if (r.element_is(miniply::kPLYFaceElement) && r.load_element() &&
                   r.find_indices(element_indices)) {
            bool polys = r.requires_triangulation(element_indices[0]);
            if (polys) {
                indices.resize(r.num_triangles(element_indices[0]) * 3);
                r.extract_triangles(element_indices[0],
                                    (float *)vertices.data(), vertices.size(),
                                    miniply::PLYPropertyType::Int,
                                    indices.data());
            } else {
                indices.resize(r.num_triangles(element_indices[0]) * 3);
                r.extract_list_property(element_indices[0],
                                        miniply::PLYPropertyType::Int,
                                        indices.data());
            }
            faces_loaded = true;
        }
        if (vertices_loaded && faces_loaded) {
            break;
        }
        r.next_element();
    }
    return {vertices, indices};
}

inline Mesh load_mesh(const std::filesystem::path &file_path,
                      const int abstracted_shape, const bool y_up,
                      const bool x_front) {
    std::string file_extension = file_path.filename().extension();
    std::pair<std::vector<glm::vec3>, std::vector<int>> loaded_file;
    std::vector<glm::vec3> ps;
    std::vector<int> indices;
    if (file_extension == ".obj") {
        loaded_file = load_obj_file(file_path);
    } else if (file_extension == ".ply") {
        loaded_file = load_ply_file(file_path);
    } else {
        return {};
    }
    ps.resize(loaded_file.first.size());
    indices = loaded_file.second;

    glm::vec3 bb_lowest = loaded_file.first[0];
    glm::vec3 bb_highest = loaded_file.first[0];
    for (int i = 1; i < ps.size(); ++i) {
        bb_highest.x = std::max(bb_highest.x, loaded_file.first[i].x);
        bb_highest.y = std::max(bb_highest.y, loaded_file.first[i].y);
        bb_highest.z = std::max(bb_highest.z, loaded_file.first[i].z);
        bb_lowest.x = std::min(bb_lowest.x, loaded_file.first[i].x);
        bb_lowest.y = std::min(bb_lowest.y, loaded_file.first[i].y);
        bb_lowest.z = std::min(bb_lowest.z, loaded_file.first[i].z);
    }
    glm::vec3 bb_center = (bb_highest + bb_lowest) / glm::vec3(2.0);
    glm::vec3 bb_dimension = glm::abs(bb_highest - bb_lowest);
    float bb_diag_size = glm::length(bb_highest - bb_lowest);

    for (int i = 0; i < ps.size(); ++i) {
        if (y_up) {
            ps[i].y = loaded_file.first[i].y - bb_center.y;
            if (x_front) {
                ps[i].x = loaded_file.first[i].x - bb_center.x;
                ps[i].z = loaded_file.first[i].z - bb_center.z;
            } else {
                ps[i].x = loaded_file.first[i].z - bb_center.z;
                ps[i].z = loaded_file.first[i].x - bb_center.x;
            }
        } else {
            ps[i].y = loaded_file.first[i].z - bb_center.z;
            if (x_front) {
                ps[i].x = loaded_file.first[i].x - bb_center.x;
                ps[i].z = loaded_file.first[i].y - bb_center.y;
            } else {
                ps[i].x = loaded_file.first[i].y - bb_center.y;
                ps[i].z = loaded_file.first[i].x - bb_center.x;
            }
        }
    }
    Mesh mesh;
    std::vector<glm::vec3> vns(ps.size(), glm::vec3{});
    for (int i = 0; i < indices.size(); i += 3) {
        glm::vec3 u = ps[indices[i + 1]] - ps[indices[i]],
                  v = ps[indices[i + 2]] - ps[indices[i]];
        glm::vec3 n = glm::cross(u, v);
        vns[indices[i]] += n;
        vns[indices[i + 1]] += n;
        vns[indices[i + 2]] += n;
    }
    for (int i = 0; i < ps.size(); ++i) {
        vns[i] = glm::normalize(vns[i]);
    }
    std::vector<glm::vec3> ans(ps.size(), glm::vec3{});
    std::vector<Vertex> vertices(ps.size());
    if (abstracted_shape == 0) { // smooth
        std::vector<glm::vec3> tns = vns;
        const int iterations = 4;
        for (int it = 0; it < iterations; ++it) {
            for (int i = 0; i < indices.size(); i += 3) {
                ans[indices[i]] += tns[indices[i + 1]] + tns[indices[i + 2]];
                ans[indices[i + 1]] += tns[indices[i]] + tns[indices[i + 2]];
                ans[indices[i + 2]] += tns[indices[i]] + tns[indices[i + 1]];
            }
            for (int i = 0; i < ps.size(); ++i) {
                ans[i] = glm::normalize(ans[i]);
                tns[i] = ans[i];
            }
        }
    } else if (abstracted_shape == 1) { // ellipse
        for (int i = 0; i < ps.size(); ++i) {
            ans[i] = glm::normalize(glm::normalize(ps[i]) * bb_dimension);
        }
    } else if (abstracted_shape == 2) { // cylinder
        for (int i = 0; i < ps.size(); ++i) {
            ans[i] = ps[i];
            if (bb_dimension.x > bb_dimension.y &&
                bb_dimension.x > bb_dimension.z) {
                ans[i].x = 0;
            } else if (bb_dimension.y > bb_dimension.z &&
                       bb_dimension.y > bb_dimension.x) {
                ans[i].y = 0;
            } else if (bb_dimension.z > bb_dimension.x &&
                       bb_dimension.z > bb_dimension.y) {
                ans[i].z = 0;
            }
            ans[i] = glm::normalize(ans[i]);
        }
    } else if (abstracted_shape == 3) { // sphere
        for (int i = 0; i < ps.size(); ++i) {
            ans[i] = glm::normalize(ps[i]);
        }
    }
    for (int i = 0; i < ps.size(); ++i) {
        vertices[i] = {ps[i] / bb_diag_size, vns[i], ans[i]};
    }
    return {vertices, indices};
}
} // namespace xtr
