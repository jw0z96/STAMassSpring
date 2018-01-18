#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// The output textures that make up our gBuffer
layout (location=0) out vec3 fragWSPosition;
layout (location=1) out vec3 fragWSNormal;
layout (location=2) out vec3 fragAlbedo;
layout (location=3) out vec2 fragMetalRough;

uniform int u_index;

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

ivec3 getIndex(int _index)
{
	int z = _index % u_sizeZ;
	int y = (_index / u_sizeZ) % u_sizeY;
	int x = _index / (u_sizeY * u_sizeZ);
	return ivec3(x, y, z);
}

void main()
{
	// world space position out
	vec3 indexColour = getIndex(u_index);
	indexColour /= 4.0;

	fragAlbedo = indexColour;
	fragWSPosition = WorldPos;
	fragWSNormal = Normal;
	fragMetalRough = vec2(0.0, 0.1);
}
