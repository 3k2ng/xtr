#version 330 core
layout(location = 0) out vec4 z_buffer;
layout(location = 1) out vec4 obam;

in vec3 frag_position;
in vec3 frag_normal;

uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;
uniform bool uni_use_obam;
uniform bool uni_use_sbam;
uniform float uni_r;
uniform float uni_s;

vec3 light_dir = normalize(vec3(1., 1., 1.));

void main()
{
    if (uni_use_obam || uni_use_sbam) {
        vec3 N = normalize(frag_normal);
        // vec3 V = normalize(frag_position - uni_camera_pos); // Ray from camera position to frag position (NOT CAMERA DIR)
        vec3 V = normalize(uni_camera_dir);
        float NdotV = dot(N, V);
        float D = pow(abs(NdotV), uni_r);
        z_buffer = vec4(vec3(D), 1.f);
        if (uni_use_sbam) { // Specular

            obam = vec4(vec3(dot(frag_normal, light_dir)), 1.); // light
        } else { // Near-Silhouette
            obam = vec4(vec3(dot(frag_normal, light_dir)), 1.); // light
        }
    } else {
        z_buffer = vec4(vec3(length(frag_position - uni_camera_pos)), 1.);
        obam = vec4(vec3(dot(frag_normal, light_dir)), 1.); // light
    }
}
