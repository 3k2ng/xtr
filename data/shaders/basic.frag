#version 330 core
layout(location = 0) out vec4 frag_color;

in vec3 frag_position;
in vec3 frag_normal;

uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;

void main()
{
    frag_color = vec4(0.5 * frag_normal + vec3(0.5), 1.);
}
