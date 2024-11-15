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
                      const bool calculate_vertex_normal,
                      const bool y_up = true, const bool x_front = true) {
    std::string file_extension = file_path.filename().extension();
    std::pair<std::vector<glm::vec3>, std::vector<int>> loaded_file;
    std::vector<glm::vec3> ps;
    std::vector<int> i_p;
    if (file_extension == ".obj") {
        loaded_file = load_obj_file(file_path);
    } else if (file_extension == ".ply") {
        loaded_file = load_ply_file(file_path);
    } else {
        return {};
    }
    ps.resize(loaded_file.first.size());

    glm::vec3 bb_lowest  = loaded_file.first[0];
    glm::vec3 bb_highest = loaded_file.first[0];
    for (int i = 1; i < ps.size(); ++i) {
        bb_highest.x = std::max(bb_highest.x, loaded_file.first[i].x);
        bb_highest.y = std::max(bb_highest.y, loaded_file.first[i].y);
        bb_highest.z = std::max(bb_highest.z, loaded_file.first[i].z);
        bb_lowest.x = std::min(bb_lowest.x, loaded_file.first[i].x);
        bb_lowest.y = std::min(bb_lowest.y, loaded_file.first[i].y);
        bb_lowest.z = std::min(bb_lowest.z, loaded_file.first[i].z);
    }
    glm::vec3 bb_center = (bb_highest - bb_lowest) / glm::vec3(2.0);
    
    for (int i = 0; i < ps.size(); ++i) {
        if (y_up) {
            ps[i].y = loaded_file.first[i].y;
            if (x_front) {
                ps[i].x = loaded_file.first[i].x;
                ps[i].z = loaded_file.first[i].z;
            } else {
                ps[i].x = loaded_file.first[i].z;
                ps[i].z = loaded_file.first[i].x;
            }
        } else {
            ps[i].y = loaded_file.first[i].z;
            if (x_front) {
                ps[i].x = loaded_file.first[i].x;
                ps[i].z = loaded_file.first[i].y;
            } else {
                ps[i].x = loaded_file.first[i].y;
                ps[i].z = loaded_file.first[i].x;
            }
        }
    }
    i_p = loaded_file.second;
    if (calculate_vertex_normal) {
        std::vector<glm::vec3> vns(ps.size(), glm::vec3{});
        for (int i = 0; i < i_p.size(); i += 3) {
            glm::vec3 u = ps[i_p[i + 1]] - ps[i_p[i]],
                      v = ps[i_p[i + 2]] - ps[i_p[i]];
            glm::vec3 n = glm::cross(u, v);
            vns[i_p[i]] += n;
            vns[i_p[i + 1]] += n;
            vns[i_p[i + 2]] += n;
        }
        std::vector<Vertex> vertices(ps.size());
        for (int i = 0; i < ps.size(); ++i) {
            vns[i] = glm::normalize(vns[i]);
            vertices[i] = {.position=ps[i]-bb_center, .normal=vns[i]};
        }
        return {vertices, i_p};
    } else {
        auto cmp = [](const glm::vec3 lhs, const glm::vec3 rhs) {
            return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y) ||
                   (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z);
        };
        std::vector<glm::vec3> fns;
        std::map<glm::vec3, int, decltype(cmp)> fn2id(cmp);
        std::vector<int> i_fn(i_p.size());
        for (int i = 0; i < i_fn.size(); i += 3) {
            glm::vec3 u = ps[i_p[i + 1]] - ps[i_p[i]],
                      v = ps[i_p[i + 2]] - ps[i_p[i]];
            glm::vec3 fn = glm::normalize(glm::cross(u, v));
            if (fn2id.find(fn) == fn2id.end()) {
                fn2id[fn] = fns.size();
                fns.push_back(fn);
            }
            i_fn[i] = fn2id[fn];
            i_fn[i + 1] = fn2id[fn];
            i_fn[i + 2] = fn2id[fn];
        }
        std::vector<Vertex> vertices;
        std::vector<int> indices(i_p.size());
        std::map<std::pair<int, int>, int> i2i;
        for (int i = 0; i < indices.size(); ++i) {
            if (i2i.find({i_p[i], i_fn[i]}) == i2i.end()) {
                i2i[{i_p[i], i_fn[i]}] = vertices.size();
                vertices.push_back({ps[i_p[i]], fns[i_fn[i]]});
            }
            indices[i] = i2i[{i_p[i], i_fn[i]}];
        }
        return {vertices, indices};
    }
}
} // namespace xtr
