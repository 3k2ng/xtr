#version 330 core
out vec4 frag_color;

in vec3 frag_position;
in vec3 frag_normal;

uniform sampler2D uni_texture;

void main()
{
    frag_color = vec4(0.5 * frag_normal + vec3(0.5), 1.);
}
