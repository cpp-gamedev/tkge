#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aUv;

layout (location = 0) out vec4 outColour;

void main()
{
	outColour = aColour;

	gl_Position = vec4(aPos, 0.0, 1.0);
}
