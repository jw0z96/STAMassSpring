#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// The output textures that make up our gBuffer
layout (location=0) out vec3 fragWSPosition;
layout (location=1) out vec3 fragWSNormal;
layout (location=2) out vec3 fragAlbedo;
layout (location=3) out vec2 fragMetalRough;

uniform vec3 camPos;
uniform vec3 diffuse;

void main()
{
	fragAlbedo = diffuse;
	fragWSPosition = WorldPos;
	fragWSNormal = Normal;
	fragMetalRough = vec2(0.0, 0.1);
}
