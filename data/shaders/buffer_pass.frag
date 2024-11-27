#version 330 core
layout(location = 0) out vec4 z_buffer;
layout(location = 1) out vec4 obam;

in vec3 frag_position;
in vec3 frag_normal;

uniform bool uni_dof;
uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;

vec3 light_dir = normalize(vec3(1., 1., 1.));

void main()
{
    if (uni_dof) {
        z_buffer = vec4(vec3(length(frag_position - uni_camera_pos)), 1.);
    }
    else {
        z_buffer = vec4(vec3(dot(normalize(uni_camera_dir), frag_position - uni_camera_pos)), 1.);
    }
    obam = vec4(vec3(abs(dot(frag_normal, uni_camera_dir))), 1.);
    obam = vec4(vec3(dot(frag_normal, light_dir)), 1.);
}
