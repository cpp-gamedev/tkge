#version 450 core

layout (location = 0) out vec4 outColour;

void main()
{
	const vec2 verts[] =
	{
		vec2(-0.5, -0.5),
		vec2(0.5, -0.5),
		vec2(0.0, 0.5),
	};


	const vec4 colours[] =
	{
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(0.0, 1.0, 0.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
	};

	const vec4 vertex = vec4(verts[gl_VertexIndex], 0.0, 1.0);
	outColour = colours[gl_VertexIndex];

	gl_Position = vertex;
}
