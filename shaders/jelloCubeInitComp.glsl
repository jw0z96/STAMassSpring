#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0, rgba8) uniform image1D u_massPointsPositionTex;

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

uniform vec3 u_bottomLeft;
uniform vec3 u_step;

int getIndex()
{
	return int(
		(gl_GlobalInvocationID.z * u_sizeX * u_sizeY)
		+ (gl_GlobalInvocationID.y * u_sizeX)
		+ gl_GlobalInvocationID.x
		);
}

void main()
{
	if (gl_GlobalInvocationID.x >= u_sizeX ||
		gl_GlobalInvocationID.y >= u_sizeY ||
		gl_GlobalInvocationID.z >= u_sizeZ)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec3 pos = writePos * u_step + u_bottomLeft;

	imageStore(u_massPointsPositionTex, getIndex(), vec4(pos, 0.0));
}
