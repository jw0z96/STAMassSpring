#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (binding = 0, offset = 0) uniform atomic_uint springCount;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct Mass
{
	vec4 position;
	vec4 velocity;
	vec4 force;
};

struct Spring
{
	uint start;
	uint end;
	float restingLength;
	vec4 velocity;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	Mass masses[];
};

layout (std430, binding = 1) buffer springsBuffer
{
	Spring springs[];
};

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

uniform vec3 u_bottomLeft;
uniform vec3 u_step;

uniform bool u_springWrite;

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

void addSpring(ivec3 _start, ivec3 _end)
{
	uint springIndex = atomicCounterIncrement(springCount);

	if (u_springWrite)
	{
		int startIndex = getIndex(_start);
		int endIndex = getIndex(_end);

		vec3 startPos = masses[startIndex].position.xyz;
		vec3 endPos = masses[endIndex].position.xyz;
		vec3 diff = endPos - startPos;
		float initialBounce = 1.0;
		float restingLength = initialBounce * length(diff);

		springs[springIndex].velocity = vec4(0.0);
		springs[springIndex].start = startIndex;
		springs[springIndex].end = endIndex;
		springs[springIndex].restingLength = restingLength;
	}
}

void main()
{
	if (gl_GlobalInvocationID.x >= u_sizeX ||
		gl_GlobalInvocationID.y >= u_sizeY ||
		gl_GlobalInvocationID.z >= u_sizeZ)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);
	int currentIndex = getIndex(writePos);

	if (!u_springWrite)
	{
		// store the initial position of the current mass
		vec3 pos = writePos * u_step + u_bottomLeft;
		masses[currentIndex].position = vec4(pos, 0.0);
		masses[currentIndex].velocity = vec4(0.0);
		// masses[currentIndex].force = vec4(0.0, u_mass * -u_gravity, 0.0, 0.0);
		masses[currentIndex].force = vec4(0.0);
	}

	// create / count structural springs
	if ((writePos.x + 1) < u_sizeX) // connect (i,j,k) to (i+1, j, k)
		addSpring(writePos, writePos + ivec3(1, 0, 0));
	if ((writePos.y + 1) < u_sizeY) // connect (i,j,k) to (i, j+1, k)
		addSpring(writePos, writePos + ivec3(0, 1, 0));
	if ((writePos.z + 1) < u_sizeZ) // connect (i,j,k) to (i, j, k+1)
		addSpring(writePos, writePos + ivec3(0, 0, 1));

	// create / count bend springs
	if ((writePos.x + 2) < u_sizeX) // connect (i,j,k) to (i+2, j, k)
		addSpring(writePos, writePos + ivec3(2, 0, 0));
	if ((writePos.y + 2) < u_sizeY) // connect (i,j,k) to (i, j+2, k)
		addSpring(writePos, writePos + ivec3(0, 2, 0));
	if ((writePos.z + 2) < u_sizeZ) // connect (i,j,k) to (i, j, k+2)
		addSpring(writePos, writePos + ivec3(0, 0, 2));

	// create / count shear springs
	if ((writePos.x + 1) < u_sizeX && (writePos.y + 1) < u_sizeY) // connect (i,j,k) to (i+1,j+1,k)
		addSpring(writePos, writePos + ivec3(1, 1, 0));
	if ((writePos.x + 1) < u_sizeX && (writePos.z + 1) < u_sizeZ) // connect (i,j,k) to (i+1,j,k+1)
		addSpring(writePos, writePos + ivec3(1, 0, 1));
	if ((writePos.x + 1) < u_sizeX && writePos.z > 0) // connect (i,j,k) to (i+1,j,k-1)
		addSpring(writePos, writePos + ivec3(1, 0, -1));
	if ((writePos.x + 1) < u_sizeX && writePos.y > 0) // connect (i,j,k) to (i+1,j-1,k)
		addSpring(writePos, writePos + ivec3(1, -1, 0));
	if ((writePos.y + 1) < u_sizeY && (writePos.z + 1) < u_sizeZ) // connect (i,j,k) to (i,j+1,k+1)
		addSpring(writePos, writePos + ivec3(0, 1, 1));
	if ((writePos.y + 1) < u_sizeY && writePos.z > 0) // connect (i,j,k) to (i,j+1,k-1)
		addSpring(writePos, writePos + ivec3(0, 1, -1));
}
