#version 330 core
layout(location = 0) out vec4 frag_color;

in vec2 uv;

uniform vec2 uni_screen_size;

uniform sampler2D uni_position;
uniform sampler2D uni_normal;
uniform sampler2D uni_id_map;
uniform sampler2D uni_tonemap;

uniform int uni_id;

uniform vec3 uni_camera_pos;
uniform vec3 uni_camera_dir;

uniform int uni_outline_type;
uniform vec3 uni_outline_col;
uniform float uni_outline_thr;
uniform int uni_outline_id_fac;
uniform float uni_outline_normal_fac;
uniform float uni_outline_position_fac;
uniform float uni_outline_edge_fac;

// Helper function to calculate the desired weight value for each pixel
// This weight value is based on ID, position and normal.
// Then, the nearby weight values are calculated and convolved with the desired operators.
void calculate_sample(int x_offset, int y_offset, out vec3 sample )
{
    // Account for ID
    int sampled_id = int(texture(
                uni_id_map,
                vec2(
                    uv.x - (x_offset / uni_screen_size.x),
                    uv.y - (y_offset / uni_screen_size.y)
                )
            ).x);
    sample = vec3(float(sampled_id))* uni_outline_id_fac;
// Account for position
sample += texture(
    uni_position,
    vec2(
        uv.x-(x_offset/uni_screen_size.x),
        uv.y-(y_offset/uni_screen_size.y)
    )
). xyz * uni_outline_position_fac;
// Account for normal
sample += texture(
    uni_normal,
    vec2(
        uv.x-(x_offset/uni_screen_size.x),
        uv.y-(y_offset/uni_screen_size.y)
    )
). xyz * uni_outline_normal_fac;
}

void main()
{
    int id = int(texture(uni_id_map, uv).x);
    if (uni_id != id && uni_outline_type <= 1) discard;

    // temp variables for storing horizontal and vertical difference vectors (for the convolution-based outline methods)
    vec3 horizontal = vec3(0.f);
    vec3 vertical = vec3(0.f);

    // naive outline method (near-silhouette)
    if (uni_outline_type == 1) {
        // retrieve normal from the G-buffer
        vec3 normal = texture(uni_normal, uv).xyz;
        // take dot product between normal and view vector
        float outline = abs(dot(normal, uni_camera_dir));
        // if outline is close to 0, that means this pixel is close to the silhouette of the object
        if (outline < uni_outline_thr) frag_color = vec4(uni_outline_col, 1.f);
        else discard;
    }
    // edge detection method (roberts cross)
    // source: https://ameye.dev/notes/rendering-outlines/
    else if (uni_outline_type == 2) {
        // we need to sample some pixels around this one
        vec3 samples[4];
        // sample the 4 pixels on the adjacent diagonals from this pixel
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                int x_offset = (i * 2) - 1;
                int y_offset = (j * 2) - 1;
                int array_index = j + i * 2;
                calculate_sample(x_offset, y_offset, samples[array_index]);
            }
        }

        // convolve the samples using the roberts cross operator
        horizontal += samples[0] * 1.f; // top left (factor +1)
        horizontal += samples[3] * -1.f; // bottom right (factor -1)
        vertical += samples[2] * -1.f; // bottom left (factor -1)
        vertical += samples[1] * 1.f; // top right (factor +1)

        // calculate total difference for this pixel
        float edge = sqrt(dot(horizontal, horizontal) + dot(vertical, vertical));

        // if difference is significant enough, it is part of the outline
        if (edge > 1.f - uni_outline_thr) frag_color = vec4(uni_outline_col, uni_outline_edge_fac);
        else discard;
    }
    // edge detection method (sobel operator)
    // source: https://ameye.dev/notes/rendering-outlines/
    else if (uni_outline_type == 3) {
        // we need to sample some pixels around this one
        vec3 samples[9];
        // sample the 9 pixels surrounding this pixel
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int x_offset = i - 1;
                int y_offset = j - 1;
                int array_index = j + i * 3;
                calculate_sample(x_offset, y_offset, samples[array_index]);
            }
        }

        // convolve the samples using the sobel operator
        horizontal += samples[0] * 1.f; // top left (factor +1)
        horizontal += samples[2] * -1.f; // top right (factor -1)
        horizontal += samples[3] * 2.f; // center left (factor +2)
        horizontal += samples[5] * -2.f; // center right (factor -2)
        horizontal += samples[6] * 1.f; // bottom left (factor +1)
        horizontal += samples[8] * -1.f; // bottom right (factor -1)
        vertical += samples[0] * 1.f; // top left (factor +1)
        vertical += samples[1] * 2.f; // top center (factor +2)
        vertical += samples[2] * 1.f; // top right (factor +1)
        vertical += samples[6] * -1.f; // bottom left (factor -1)
        vertical += samples[7] * -2.f; // bottom center (factor -2)
        vertical += samples[8] * -1.f; // bottom right (factor -1)

        // calculate total difference for this pixel
        float edge = sqrt(dot(horizontal, horizontal) + dot(vertical, vertical));

        // if difference is significant enough, it is part of the outline
        if (edge > 1.f - uni_outline_thr) frag_color = vec4(uni_outline_col, uni_outline_edge_fac);
        else discard;
    }
    else discard;
}

// if (edge > 1.f - uni_outline_thr) frag_color = vec4(uni_outline_col, clamp(edge * uni_outline_edge_fac, 0.f, 1.f));

