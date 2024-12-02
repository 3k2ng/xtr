#version 330 core
layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
// layout(location = 2) in vec3 vert_abstracted_normal;

out vec3 frag_position;
out vec3 frag_normal;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

uniform float uni_norm_fac;

void main()
{
    gl_Position = uni_projection * uni_view * uni_model * vec4(vert_position, 1.0);
    frag_position = vec3(uni_model * vec4(vert_position, 1.0));
    vec3 normal = vec3(uni_model * vec4(vert_normal, 1.0));
    if (uni_norm_fac > 0.f) {
        vec3 vert_abstracted_normal = vec3(1.f, 0.f, 0.f);
        vec3 abstracted_normal = vec3(uni_model * vec4(vert_abstracted_normal, 1.0));
        frag_normal = normal * (1.f - uni_norm_fac) + abstracted_normal * (uni_norm_fac);
    } else {
        frag_normal = normal;
    }
}
