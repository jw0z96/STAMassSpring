#version 430

#extension GL_NV_shader_atomic_float : enable

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct State
{
	vec4 position;
	vec4 velocity;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	State masses[];
};

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

uniform float u_currentTime;
uniform float u_timeStep;

uniform float u_mass;
uniform float u_gravity;

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
	if (gl_GlobalInvocationID.x >= u_sizeX ||
		gl_GlobalInvocationID.y >= u_sizeY ||
		gl_GlobalInvocationID.z >= u_sizeZ)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);
	int currentIndex = getIndex(writePos);

	// if (currentIndex == (u_sizeX * u_sizeY * u_sizeZ) - 1 )
	// 	return;

	// if (currentIndex == (u_sizeX * u_sizeY) - 1)
	// 	return;

	// if (currentIndex == (u_sizeX * u_sizeY) - u_sizeX)
	// 	return;

	// if (currentIndex == (u_sizeX * u_sizeY * u_sizeZ) - u_sizeZ )
	// 	return;

	masses[currentIndex].velocity.xyz = u_timeStep * vec3(0.0, u_mass * -u_gravity, 0.0);

	masses[currentIndex].position.xyz += masses[currentIndex].velocity.xyz;

	masses[currentIndex].position.y = max(masses[currentIndex].position.y, 0.0);

	// masses[currentIndex].velocity.xyz = vec3(0.0);

	// atomicAdd(masses[currentIndex].position.x, masses[currentIndex].velocity.x);
	// atomicAdd(masses[currentIndex].position.y, masses[currentIndex].velocity.y);
	// atomicAdd(masses[currentIndex].position.z, masses[currentIndex].velocity.z);
}
