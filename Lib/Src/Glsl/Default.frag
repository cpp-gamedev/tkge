#version 450 core

layout (set = 0, binding = 2) uniform sampler2D tex;

layout (location = 0) in vec4 inColour;
layout (location = 1) in vec2 inUv;

layout (location = 0) out vec4 outColour;

void main()
{
	outColour = inColour * texture(tex, inUv);
}
