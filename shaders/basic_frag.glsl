#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// The output textures that make up our gBuffer
layout (location=0) out vec4 fragColour;

uniform vec3 camPos;
uniform vec3 diffuse;

void main()
{
	float ndotl = dot(WorldPos, camPos);
	fragColour = vec4(diffuse, 1.0);
}
