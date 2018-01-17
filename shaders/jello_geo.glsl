#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 8) out;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct Mass
{
	vec4 position;
	vec4 velocity;
	vec4 force;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	Mass masses[];
};

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

uniform mat4 MVP;

flat out int axis;

int getIndex(ivec3 pos)
{
	return int(
		(pos.z * u_sizeX * u_sizeY)
		+ (pos.y * u_sizeX)
		+ pos.x
		);
}

void main()
{
	int quadID = gl_PrimitiveIDIn;

	int quadsXY = (u_sizeX - 1) * (u_sizeY - 1);
	int quadsXZ = (u_sizeX - 1) * (u_sizeZ - 1);
	int quadsYZ = (u_sizeY - 1) * (u_sizeZ - 1);

	// draw all of the X - Y quads
	if (quadID < quadsXY)
	{
		int i = int(mod(quadID, (u_sizeX - 1)));
		int j = int(floor((quadID - i) / (u_sizeY - 1)));
		axis = 0;
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, j, 0))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, j + 1, 0))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, j, 0))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, j + 1, 0))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
		// and for the opposite side
		axis = 1;
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, j, (u_sizeZ - 1)))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, j, (u_sizeZ - 1)))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, j + 1, (u_sizeZ - 1)))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, j + 1, (u_sizeZ - 1)))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
	}
	// draw all of the Y - Z quads
	if (quadID < quadsXY + quadsYZ)
	{
		int id = quadID - quadsXY;
		int i = int(mod(id, (u_sizeZ - 1)));
		int j = int(floor((id - i) / (u_sizeY - 1)));

		axis = 2;
		gl_Position = MVP * vec4(masses[getIndex(ivec3(0, i, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(0, i, j + 1))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(0, i + 1, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(0, i + 1, j + 1))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
		// and for the opposite side
		axis = 3;
		gl_Position = MVP * vec4(masses[getIndex(ivec3((u_sizeX - 1), i, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3((u_sizeX - 1), i + 1, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3((u_sizeX - 1), i, j + 1))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3((u_sizeX - 1), i + 1, j + 1))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
	}
	// draw all of the X - Z quads
	else
	{
		int id = quadID - (quadsXY + quadsYZ);

		int i = int(mod(id, (u_sizeX - 1)));
		int j = int(floor((id - i) / (u_sizeZ - 1)));

		axis = 4;
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, 0, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, 0, j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, 0, j + 1))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, 0, j + 1))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
		// and for the opposite side
		axis = 5;
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, (u_sizeY - 1), j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i, (u_sizeY - 1), j + 1))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, (u_sizeY - 1), j))].position.xyz, 1.0);
		EmitVertex();
		gl_Position = MVP * vec4(masses[getIndex(ivec3(i + 1, (u_sizeY - 1), j + 1))].position.xyz, 1.0);
		EmitVertex();
		EndPrimitive();
	}
}
