#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform sampler2D uni_texture;

void main()
{
    frag_color = texture(uni_texture, uv);
}
