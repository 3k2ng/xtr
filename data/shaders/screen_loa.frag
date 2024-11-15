#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform float uni_z_min;
uniform float uni_z_max;

uniform sampler2D uni_z_buffer;
uniform sampler2D uni_obam;
uniform sampler2D uni_tonemap;

void main()
{
    float z = texture(uni_z_buffer, uv).x;
    if (z <= 0.) discard;
    float dbam = log(z / uni_z_min) / log(uni_z_max / uni_z_min);
    float obam = texture(uni_obam, uv).x;
    frag_color = texture(uni_tonemap, vec2(obam, dbam));
}
