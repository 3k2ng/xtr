#version 330 core
layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_texcoord;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texcoord;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

void main()
{
    gl_Position = uni_projection * uni_view * uni_model * vec4(vert_position, 1.0);
    frag_position = vec3(uni_model * vec4(vert_position, 1.0));
    frag_normal = vec3(uni_model * vec4(vert_normal, 1.0));
    frag_texcoord = vert_texcoord;
}
