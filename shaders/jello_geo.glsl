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

smooth out vec3 f_normal;
smooth out vec3 f_pos;

int getIndex(ivec3 pos)
{
	return int(
		(pos.z * u_sizeX * u_sizeY)
		+ (pos.y * u_sizeX)
		+ pos.x
		);
}

// void emitQuad(vec4 _verts[4])
// {
// 	// calculate the normal for our first triangle in the quad;
// 	// vec3 wsNormal = normalize(
// 	// 	cross(_verts[1].xyz - _verts[0].xyz, _verts[2].xyz - _verts[0].xyz) +
// 	// 	cross(_verts[2].xyz - _verts[1].xyz, _verts[3].xyz - _verts[1].xyz));

// 	for (int i = 0; i < 4; ++i)
// 	{
// 		// f_normal = wsNormal;
// 		// f_pos = _verts[i].xyz;
// 		gl_Position = MVP * _verts[i];
// 		EmitVertex();
// 	}
// 	EndPrimitive;
// }

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
		vec4 vertices[4];
		vertices[0] = vec4(masses[getIndex(ivec3(i, j, 0))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3(i, j + 1, 0))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3(i + 1, j, 0))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3(i + 1, j + 1, 0))].position.xyz, 1.0);
		vec3 wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
		// and for the opposite side
		axis = 1;
		vertices[0] = vec4(masses[getIndex(ivec3(i, j, (u_sizeZ - 1)))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3(i + 1, j, (u_sizeZ - 1)))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3(i, j + 1, (u_sizeZ - 1)))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3(i + 1, j + 1, (u_sizeZ - 1)))].position.xyz, 1.0);
		wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
	}
	else if (quadID < quadsXY + quadsYZ) // draw all of the Y - Z quads
	{
		int id = quadID - quadsXY;
		int i = int(mod(id, (u_sizeZ - 1)));
		int j = int(floor((id - i) / (u_sizeY - 1)));
		axis = 2;
		vec4 vertices[4];
		vertices[0] = vec4(masses[getIndex(ivec3(0, i, j))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3(0, i, j + 1))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3(0, i + 1, j))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3(0, i + 1, j + 1))].position.xyz, 1.0);
		vec3 wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
		// and for the opposite side
		axis = 3;
		vertices[0] = vec4(masses[getIndex(ivec3((u_sizeX - 1), i, j))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3((u_sizeX - 1), i + 1, j))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3((u_sizeX - 1), i, j + 1))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3((u_sizeX - 1), i + 1, j + 1))].position.xyz, 1.0);
		wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
	}
	else if (quadID < quadsXY + quadsYZ + quadsXZ) // draw all of the X - Z quads
	{
		int id = quadID - (quadsXY + quadsYZ);
		int i = int(mod(id, (u_sizeX - 1)));
		int j = int(floor((id - i) / (u_sizeZ - 1)));
		axis = 4;
		vec4 vertices[4];
		vertices[0] = vec4(masses[getIndex(ivec3(i, 0, j))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3(i + 1, 0, j))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3(i, 0, j + 1))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3(i + 1, 0, j + 1))].position.xyz, 1.0);
		vec3 wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
		// and for the opposite side
		axis = 5;
		vertices[0] = vec4(masses[getIndex(ivec3(i, (u_sizeY - 1), j))].position.xyz, 1.0);
		vertices[1] = vec4(masses[getIndex(ivec3(i, (u_sizeY - 1), j + 1))].position.xyz, 1.0);
		vertices[2] = vec4(masses[getIndex(ivec3(i + 1, (u_sizeY - 1), j))].position.xyz, 1.0);
		vertices[3] = vec4(masses[getIndex(ivec3(i + 1, (u_sizeY - 1), j + 1))].position.xyz, 1.0);
		wsNormal = normalize(
			cross(vertices[1].xyz - vertices[0].xyz, vertices[2].xyz - vertices[0].xyz) +
			cross(vertices[1].xyz - vertices[2].xyz, vertices[3].xyz - vertices[2].xyz));
		for (int i = 0; i < 4; ++i)
		{
			f_normal = wsNormal;
			f_pos = vertices[i].xyz;
			gl_Position = MVP * vertices[i];
			EmitVertex();
		}
		EndPrimitive();
	}
	else
	{
	}
}
