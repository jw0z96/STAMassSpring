#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// The output textures that make up our gBuffer
layout (location=0) out vec4 fragColour;

void main()
{
	// world space position out
	fragColour = vec4(WorldPos, 1.0);
}
