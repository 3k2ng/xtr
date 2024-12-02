#version 330 core
layout(location = 0) out float nl;
layout(location = 1) out float z_buffer;
layout(location = 2) out float obam;
layout(location = 3) out vec3 position;

in vec3 frag_position;
in vec3 frag_normal;

uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;

uniform bool uni_use_obam;
uniform bool uni_use_dof;
uniform bool uni_use_specular;

uniform float uni_near_silhouette_r; // near-silhouette r
uniform float uni_specular_s; // specular s

vec3 light_dir = normalize(vec3(1.));

void main()
{
    nl = dot(frag_normal, light_dir);
    if (uni_use_dof) {
        z_buffer = length(frag_position - uni_camera_pos);
    }
    else {
        z_buffer = dot(normalize(uni_camera_dir), frag_position - uni_camera_pos);
    }
    if (uni_use_specular) {
        vec3 reflected_light_dir = 2. * dot(light_dir, frag_normal) * frag_normal - light_dir;
        obam = pow(abs(dot(uni_camera_dir, reflected_light_dir)), uni_specular_s);
    }
    else {
        obam = pow(abs(dot(frag_normal, uni_camera_dir)), uni_near_silhouette_r);
    }
    position = frag_position;
}
