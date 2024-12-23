// mesh pass fragment shader
#version 330 core
layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out float id;

in vec3 frag_position;
in vec3 frag_normal;

uniform int uni_id;

void main()
{
    position = frag_position;
    normal = frag_normal;
    id = uni_id;
}
