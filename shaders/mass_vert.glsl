#version 430 core

/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 2) in vec3 inNormal;
/// @brief the in uv
layout (location = 1) in vec2 inUV;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct Mass
{
	vec4 position;
	vec4 velocity;
	vec4 force;
};

layout (std430, binding = 0) buffer massBuffer
{
	Mass masses[];
};

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform mat4 M;

uniform int u_index;

void main()
{
	TexCoords = inUV;

	// access texture to get position of mass point
	vec3 massPos = masses[u_index].position.xyz;

	// add the position to our vert position (and scale the cube down a bit...)
	float cubeScale = 0.1;
	vec3 pos = (cubeScale * inVert) + massPos;

	// pass worldspace position to fragment shader
	WorldPos = vec3(M * vec4(pos, 1.0f));

	// pass worldspace normal to fragment shader
	Normal = inNormal;

	// transform input vertex
	gl_Position = MVP * vec4(pos,1.0);
}
