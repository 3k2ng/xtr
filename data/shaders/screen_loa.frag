#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform float uni_z_min;
uniform float uni_z_max;

uniform sampler2D uni_z_buffer;

uniform sampler2D uni_obam;
uniform sampler2D uni_tonemap;

uniform bool uni_use_obam;
uniform bool uni_use_sbam;

uniform float uni_z_c;
uniform bool uni_use_dof;

void main()
{
    float z = texture(uni_z_buffer, uv).x;
    if (z <= 0.) discard;

    float obam = texture(uni_obam, uv).x;

    float dbam;
    if (uni_use_obam || uni_use_sbam) {
        z /= max(uni_z_max, 0.1);
        dbam = 1. - z;
        frag_color = texture(uni_tonemap, vec2(obam, dbam));
    } else if (uni_use_dof) {
        if (z < uni_z_c) {
            float dof_z_min = uni_z_c - uni_z_min;
            float dof_z_max = uni_z_c - uni_z_max;
            dbam = (log(z / dof_z_min) / log(dof_z_max / dof_z_min)); // this is so weird. why does it work without the 1- ?
        } else {
            float dof_z_min = uni_z_c + uni_z_min;
            float dof_z_max = uni_z_c + uni_z_max;
            dbam = (log(z / dof_z_min) / log(dof_z_max / dof_z_min));
        }
        frag_color = texture(uni_tonemap, vec2(obam, dbam));
    } else {
        dbam = log(z / uni_z_min) / log(uni_z_max / uni_z_min);
        frag_color = texture(uni_tonemap, vec2(obam, dbam));
    }
    // frag_color = vec4(vec3(obam), 1.f); // DEBUG
}
