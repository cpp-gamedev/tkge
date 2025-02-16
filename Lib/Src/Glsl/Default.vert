#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aUv;

layout (set = 0, binding = 0) uniform View
{
	mat4 matVP;
};

layout (location = 0) out vec4 outColour;

void main()
{
	outColour = aColour;

	gl_Position = matVP * vec4(aPos, 0.0, 1.0);
}
