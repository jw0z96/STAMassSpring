#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

flat in int springId[];

uniform sampler1D massPointsPositionTex;
uniform sampler1D springStartIndexTex;
uniform sampler1D springEndIndexTex;

uniform mat4 MVP;

void main()
{
	int startId = int(texelFetch(springStartIndexTex, 0, 0).x);
	vec3 startPos = texelFetch(massPointsPositionTex, startId, 0).xyz;
	gl_Position = MVP * vec4(startPos, 1.0);
	EmitVertex();

	int endId = int(texelFetch(springEndIndexTex, 100, 0).x);
	vec3 endPos = texelFetch(massPointsPositionTex, endId, 0).xyz;
	gl_Position = MVP * vec4(endPos, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(1.0, 0.0, 0.0, 1.0);
	EmitVertex();

	EndPrimitive();
}
