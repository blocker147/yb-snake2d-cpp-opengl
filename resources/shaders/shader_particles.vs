#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 offset;
layout (location = 2) in vec4 color;
layout (location = 3) in float size;

out vec4 fColor;

void main()
{
    vec3 pos = aPos * size;
    gl_Position = vec4(pos.xy + offset, 0.0, 1.0);
    fColor = color;
}