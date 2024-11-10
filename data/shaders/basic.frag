#version 330 core
out vec4 frag_color;

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texcoord;

uniform sampler2D uni_texture;

void main()
{
    frag_color = texture(uni_texture, frag_texcoord);
}
