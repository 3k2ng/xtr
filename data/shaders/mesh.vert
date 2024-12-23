// vertex shader for the mesh pass
// mesh vertex data as input
#version 330 core
layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec3 vert_abstracted_normal;

out vec3 frag_position;
out vec3 frag_normal;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

uniform float uni_normal_factor;

void main()
{
    // final rendered fragment position
    gl_Position = uni_projection * uni_view * uni_model * vec4(vert_position, 1.0);
    // position to use in the fragment shader + position buffer
    frag_position = vec3(uni_model * vec4(vert_position, 1.0));
    vec3 normal = vec3(uni_model * vec4(vert_normal, 1.0));
    vec3 abstracted_normal = vec3(uni_model * vec4(vert_abstracted_normal, 1.0));
    // combined normal, to use in the fragment shader + normal buffer
    if (uni_normal_factor > 0.f) {
        frag_normal = mix(normal, abstracted_normal, uni_normal_factor);
    } else {
        frag_normal = normal;
    }
}
