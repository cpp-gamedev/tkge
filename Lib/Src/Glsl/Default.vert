#version 450 core

struct Instance
{
	mat4 model;
	vec4 tint;
};

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aUv;

layout (set = 0, binding = 0) uniform View
{
	mat4 matVP;
};

layout (set = 0, binding = 1) readonly buffer Instances
{
	Instance instances[];
};

layout (location = 0) out vec4 outColour;
layout (location = 1) out vec2 outUv;

void main()
{
	const Instance instance = instances[gl_InstanceIndex];

	const vec4 worldPos = instance.model * vec4(aPos, 0.0, 1.0);

	outColour = aColour * instance.tint;
	outUv = aUv;

	gl_Position = matVP * worldPos;
}
