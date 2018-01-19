#version 430

#extension GL_NV_shader_atomic_float : enable

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

uniform float u_currentTime;
uniform float u_timeStep;

uniform float u_mass;
uniform float u_gravity;
uniform float u_friction;

uniform bool u_sphereCollisions;
uniform vec3 u_spherePos;
uniform float u_sphereRadius;

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
	// if (gl_GlobalInvocationID.x >= u_sizeX ||
	// 	gl_GlobalInvocationID.y >= u_sizeY ||
	// 	gl_GlobalInvocationID.z >= u_sizeZ)
	// 	return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);
	int currentIndex = getIndex(writePos);

	if (u_sphereCollisions)
	{
		if (currentIndex == (u_sizeX * u_sizeY * u_sizeZ) - 1 )
			return;

		if (currentIndex == (u_sizeX * u_sizeY) - 1)
			return;

		if (currentIndex == (u_sizeX * u_sizeY) - u_sizeX)
			return;

		if (currentIndex == (u_sizeX * u_sizeY * u_sizeZ) - u_sizeZ )
			return;
	}

	masses[currentIndex].force += vec4(0.0, u_mass * u_gravity, 0.0, 0.0);

	masses[currentIndex].velocity.xyz += u_timeStep * masses[currentIndex].force.xyz;

	masses[currentIndex].position.xyz += masses[currentIndex].velocity.xyz;

	// reset force values
	masses[currentIndex].force = vec4(0.0);

	// sphere collision
	if (u_sphereCollisions)
	{
		vec3 sphereToMass = masses[currentIndex].position.xyz - u_spherePos;
		if (length(sphereToMass) <= u_sphereRadius)
		{
			vec3 normalizedSphereToMass = normalize(sphereToMass);
			masses[currentIndex].position.xyz = u_spherePos + (normalizedSphereToMass * u_sphereRadius);
			masses[currentIndex].force.xyz += u_mass * normalizedSphereToMass;
			masses[currentIndex].velocity.xyz *= 1.0 - u_friction; // friction
		}
	}

	// ground plane collision
	if (masses[currentIndex].position.y < 0.0)
	{
		masses[currentIndex].position.y = 0.0;
		masses[currentIndex].force.xyz += u_mass * vec3(0.0, 1.0, 0.0);
		masses[currentIndex].velocity.xyz *= 1.0 - u_friction; // friction
	}

	// air resistance
	float u_velocityDamping = 1.0;
	masses[currentIndex].velocity.xyz *= 1.0 - (u_velocityDamping * u_timeStep);
}
