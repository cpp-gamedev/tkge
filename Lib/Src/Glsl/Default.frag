#version 450 core

layout (location = 0) in vec4 inColour;

layout (location = 0) out vec4 outColour;

void main()
{
	outColour = inColour;
}
