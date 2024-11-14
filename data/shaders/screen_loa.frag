#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform float uni_z_min;
uniform float uni_z_max;

uniform sampler2D uni_z_buffer;

void main()
{
    float z = texture(uni_z_buffer, uv).x;
    frag_color = vec4(vec3(1. - log(z / uni_z_min) / log(uni_z_max / uni_z_min)), 1.);
}
