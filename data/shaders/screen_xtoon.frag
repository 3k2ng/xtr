#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform sampler2D uni_nl;
uniform sampler2D uni_tonemap;
uniform sampler2D uni_z_buffer;
uniform sampler2D uni_obam;

uniform bool uni_use_obam;
uniform bool uni_use_dof;
uniform bool uni_use_specular;

uniform float uni_dbam_z_min;
uniform float uni_dbam_r;
uniform float uni_dof_z_c;

void main()
{
    float z = texture(uni_z_buffer, uv).x;
    if (z <= 0.) discard;

    float nl = texture(uni_nl, uv).x;
    float dbam;
    if (uni_use_dof) {
        if (z < uni_dof_z_c) {
            float dof_z_min = uni_dof_z_c - uni_dbam_z_min;
            float dof_z_max = uni_dof_z_c - uni_dbam_r * uni_dbam_z_min;
            dbam = 1. - log(z / dof_z_min) / log(dof_z_max / dof_z_min);
        } else {
            float dof_z_min = uni_dof_z_c + uni_dbam_z_min;
            float dof_z_max = uni_dof_z_c + uni_dbam_r * uni_dbam_z_min;
            dbam = log(z / dof_z_max) / log(dof_z_min / dof_z_max);
        }
    } else {
        dbam = 1. - log(z / uni_dbam_z_min) / log(uni_dbam_r * uni_dbam_z_min / uni_dbam_z_min);
    }
    float obam = texture(uni_obam, uv).x;
    // textures are flipped vertically
    if (uni_use_obam) {
        frag_color = texture(uni_tonemap, vec2(nl, 1. - obam));
    }
    else {
        frag_color = texture(uni_tonemap, vec2(nl, 1. - dbam));
    }
}
