#version 330 core
vec2 positions[] = vec2[](
        vec2(-3., -1.),
        vec2(1., -1.),
        vec2(1., 3.)
    );

out vec2 uv;

void main()
{
    gl_Position = vec4(positions[gl_VertexID], 0., 1.);
    uv = 0.5 * positions[gl_VertexID] + vec2(0.5);
}
