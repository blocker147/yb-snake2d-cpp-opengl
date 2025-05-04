#version 330 core

out vec4 FragColor;

uniform bool hover;
uniform bool isOver;

void main()
{
	if (isOver)
	{
		if (hover)
			FragColor = vec4(0.949, 0.537, 0.631, 1.0);
		else
			FragColor = vec4(0.678, 0.38, 0.451, 1.0);
	}
	else
	{
		if (hover)
			FragColor = vec4(0.902, 0.98, 0.318, 1.0);
		else
			FragColor = vec4(0.753, 0.788, 0.455, 1.0);
	}
}