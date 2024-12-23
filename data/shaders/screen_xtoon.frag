#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform vec2 uni_screen_size;

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

uniform vec3 uni_light_dir;

uniform bool uni_nl_halftone;
uniform float uni_dot_size;
uniform float uni_rotation;

// matrix rotation
vec2 rotate(vec2 v, float r) {
    return v * mat2(cos(r), sin(r), -sin(r), cos(r));
}

void main()
{
    int id = int(texture(uni_id_map, uv).x);
    if (uni_id != id) discard;

    vec3 position = texture(uni_position, uv).xyz;
    vec3 normal = texture(uni_normal, uv).xyz;
    float nl = dot(normal, uni_light_dir);

    // x-toon halftone
    if (uni_nl_halftone)
    {
        vec2 frag_coord = uv * uni_screen_size;

        vec2 uv_nl = rotate(round(rotate(frag_coord, uni_rotation) / uni_dot_size) * uni_dot_size, -uni_rotation);
        float d_nl = distance(frag_coord, uv_nl) * sqrt(2.0) / uni_dot_size;
        float v_nl = dot(texture(uni_normal, uv_nl / uni_screen_size).xyz, uni_light_dir);
        nl = float(d_nl < v_nl);
    }

    // level of abstraction
    if (uni_detail_mapping == 0) {
        // calculate depth of this pixel
        float z = dot(normalize(uni_camera_dir), position - uni_camera_pos);
        // in this mode, D is dependent on the depth of this pixel, compared to the user-specified z_min and r values
        float z_min = uni_dbam_z_min;
        float z_max = uni_dbam_z_min * uni_dbam_r;
        float dbam = 1. - log(z / z_min) / log(z_max / z_min);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - dbam));
    }
    // depth of field
    else if (uni_detail_mapping == 1) {
        // calculate depth of this pixel
        float z = length(position - uni_camera_pos);
        // in this mode, D is dependent on the depth of this pixel, compared to the depth of the user-specified focus point
        float dbam = 0.;
        // if this pixel is closer to the camera than the focus point...
        if (z < uni_dof_z_c) {
            float z_min_mi = uni_dof_z_c - uni_dbam_z_min;
            float z_max_mi = uni_dof_z_c - uni_dbam_r * uni_dbam_z_min;
            dbam = 1. - log(z / z_min_mi) / log(z_max_mi / z_min_mi);
        }
        // if this pixel is further from the camera than the focus point...
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
        // in this mode, D is dependent on the dot product between the normal and the view vector
        float obam = pow(abs(dot(normal, uni_camera_dir)), uni_near_silhouette_r);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - obam));
    }
    // specular
    else if (uni_detail_mapping == 3) {
        // calculate reflection vector (phong)
        vec3 reflected_light_dir = 2. * dot(uni_light_dir, normal) * normal - uni_light_dir;
        // in this mode, D is dependent on the dot product between the reflection vector and the view vector
        float obam = pow(abs(dot(uni_camera_dir, reflected_light_dir)), uni_specular_s);
        // textures are flipped vertically
        frag_color = texture(uni_tonemap, vec2(nl, 1. - obam));
    }
    else discard;
}
