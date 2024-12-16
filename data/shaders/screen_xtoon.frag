#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform sampler2D uni_position;
uniform sampler2D uni_normal;
uniform sampler2D uni_id_map;
uniform sampler2D uni_tonemap;

uniform int uni_id;

uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;

uniform int uni_detail_mapping;

uniform float uni_near_silhouette_r; // near-silhouette r
uniform float uni_specular_s; // specular s

uniform float uni_dbam_z_min;
uniform float uni_dbam_r;
uniform float uni_dof_z_c;

uniform vec3 uni_outline_col;
uniform float uni_outline_thr;

uniform vec3 uni_light_dir;

void main()
{
    int id = int(texture(uni_id_map, uv).x);
    if (uni_id != id) discard;

    vec3 position = texture(uni_position, uv).xyz;
    vec3 normal = texture(uni_normal, uv).xyz;
    float nl = dot(normal, uni_light_dir);

    // level of abstraction
    if (uni_detail_mapping == 0) {
        float z = dot(normalize(uni_camera_dir), position - uni_camera_pos);
        float z_min = uni_dbam_z_min;
        float z_max = uni_dbam_z_min * uni_dbam_r;
        float dbam = 1. - log(z / z_min) / log(z_max / z_min);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - dbam));
    }
    // depth of field
    else if (uni_detail_mapping == 1) {
        float z = length(position - uni_camera_pos);
        float dbam = 0.;
        if (z < uni_dof_z_c) {
            float z_min_mi = uni_dof_z_c - uni_dbam_z_min;
            float z_max_mi = uni_dof_z_c - uni_dbam_r * uni_dbam_z_min;
            dbam = 1. - log(z / z_min_mi) / log(z_max_mi / z_min_mi);
        }
        else {
            float z_min_pl = uni_dof_z_c + uni_dbam_z_min;
            float z_max_pl = uni_dof_z_c + uni_dbam_r * uni_dbam_z_min;
            dbam = log(z / z_max_pl) / log(z_min_pl / z_max_pl);
        }
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - dbam));
    }
    // near-silhouette
    else if (uni_detail_mapping == 2) {
        float obam = pow(abs(dot(normal, uni_camera_dir)), uni_near_silhouette_r);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - obam));
    }
    // specular
    else if (uni_detail_mapping == 3) {
        vec3 reflected_light_dir = 2. * dot(uni_light_dir, normal) * normal - uni_light_dir;
        float obam = pow(abs(dot(uni_camera_dir, reflected_light_dir)), uni_specular_s);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - obam));
    }
    else discard;


    float outline = pow(abs(dot(normal, uni_camera_dir)), 1.f);
    if (outline < uni_outline_thr) frag_color = vec4(uni_outline_col, 1.f);
}
