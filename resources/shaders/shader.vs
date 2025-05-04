#version 330 core
layout (location = 0) in float index;
layout (location = 1) in vec3 pos;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 textureCoords;

out float f_index;
out vec3 f_color;
out vec2 f_textureCoords;

void main()
{
	f_index = index;
	f_color = color;
	f_textureCoords = textureCoords;
	gl_Position = vec4(pos, 1.0);
}