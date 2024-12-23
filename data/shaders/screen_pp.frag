// post-processing shader, only comic halftone effect is available for now
#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform vec2 uni_screen_size;

uniform sampler2D uni_frame;
uniform sampler2D uni_id_map;

uniform int uni_id;

// select the post-processing effect, 0 is none, and 1 is halftone
uniform int uni_pp_effect;

// halftone parameters
uniform float uni_dot_size; // halftone max dot size
uniform float uni_rotation_c; // orientation angle of the cyan layer
uniform float uni_rotation_m; // orientation angle of the magenta layer
uniform float uni_rotation_y; // orientation angle of the yellow layer
uniform float uni_rotation_k; // orientation angle of the key layer

// https://www.rapidtables.com/convert/color/rgb-to-cmyk.html
vec4 rgb2cmyk(vec3 rgb) {
    float k = 1. - max(rgb.r, max(rgb.g, rgb.b));
    float c = (1. - rgb.r - k) / (1. - k);
    float m = (1. - rgb.g - k) / (1. - k);
    float y = (1. - rgb.b - k) / (1. - k);
    return vec4(c, m, y, k);
}

// matrix rotation
vec2 rotate(vec2 v, float r) {
    return v * mat2(cos(r), sin(r), -sin(r), cos(r));
}

float soft_threshold(float value, float threshold) {
    float v = threshold - value;
    if (v < -1.) return 0.;
    if (v > 0.) return 1.;
    return v + 1.;
}

void main() {
    // check if the id match
    int id = int(texture(uni_id_map, uv).x);
    if (uni_id != id) discard;

    if (uni_pp_effect == 0) {
        frag_color = texture(uni_frame, uv);
    }
    else if (uni_pp_effect == 1) {
        // pixel coordinate
        vec2 frag_coord = uv * uni_screen_size;

        // nearest dot location
        vec2 uv_c = rotate(round(rotate(frag_coord, uni_rotation_c) / uni_dot_size) * uni_dot_size, -uni_rotation_c);
        vec2 uv_m = rotate(round(rotate(frag_coord, uni_rotation_m) / uni_dot_size) * uni_dot_size, -uni_rotation_m);
        vec2 uv_y = rotate(round(rotate(frag_coord, uni_rotation_y) / uni_dot_size) * uni_dot_size, -uni_rotation_y);
        vec2 uv_k = rotate(round(rotate(frag_coord, uni_rotation_k) / uni_dot_size) * uni_dot_size, -uni_rotation_k);

        // distance to nearest dot
        float d_c = distance(frag_coord, uv_c);
        float d_m = distance(frag_coord, uv_m);
        float d_y = distance(frag_coord, uv_y);
        float d_k = distance(frag_coord, uv_k);

        // cmyk at dot
        float v_c = rgb2cmyk(texture(uni_frame, uv_c / uni_screen_size).rgb).x;
        float v_m = rgb2cmyk(texture(uni_frame, uv_m / uni_screen_size).rgb).y;
        float v_y = rgb2cmyk(texture(uni_frame, uv_y / uni_screen_size).rgb).z;
        float v_k = rgb2cmyk(texture(uni_frame, uv_k / uni_screen_size).rgb).w;

        // final mask
        vec3 col_c = vec3(1.) - vec3(1., 0., 0.) * soft_threshold(d_c, v_c / sqrt(2.) * uni_dot_size);
        vec3 col_m = vec3(1.) - vec3(0., 1., 0.) * soft_threshold(d_m, v_m / sqrt(2.) * uni_dot_size);
        vec3 col_y = vec3(1.) - vec3(0., 0., 1.) * soft_threshold(d_y, v_y / sqrt(2.) * uni_dot_size);
        vec3 col_k = vec3(1.) - vec3(1.) * soft_threshold(d_k, v_k / sqrt(2.) * uni_dot_size);

        // combined result
        frag_color = vec4(vec3(col_c * col_m * col_y * col_k), 1.);
    }
    else discard;
}
