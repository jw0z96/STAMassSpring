#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// The output textures that make up our gBuffer
layout (location=0) out vec4 fragColour;

uniform vec3 camPos;

void main()
{
	float ndotl = dot(WorldPos, camPos);
	vec3 colour = vec3(0.2);
	fragColour = vec4(colour, 1.0);
}
