#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (binding = 0, offset = 0) uniform atomic_uint springCount;

layout (binding = 0, rgba8) uniform image1D u_massPointsPositionTex;
layout (binding = 1, r16f) uniform writeonly image1D u_springsRestingLengthTex;
layout (binding = 2, rgba32f) uniform writeonly image1D u_springsStatePositionTex;
layout (binding = 3, rgba32f) uniform writeonly image1D u_springsStateVelocityTex;
layout (binding = 4, r16ui) uniform uimage1D u_springsStartIndexTex;
layout (binding = 5, r16ui) uniform uimage1D u_springsEndIndexTex;

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

uniform vec3 u_bottomLeft;
uniform vec3 u_step;

uniform bool u_springWrite;

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

		vec3 startPos = imageLoad(u_massPointsPositionTex, startIndex).xyz;
		vec3 endPos = imageLoad(u_massPointsPositionTex, endIndex).xyz;
		vec3 diff = endPos - startPos;
		float restingLength = length(diff);

		imageStore(u_springsRestingLengthTex, int(springIndex), vec4(restingLength));
		imageStore(u_springsStatePositionTex, int(springIndex), vec4(diff, 0.0));
		imageStore(u_springsStateVelocityTex, int(springIndex), vec4(0.0));
		imageStore(u_springsStartIndexTex, int(springIndex), uvec4(startIndex));
		imageStore(u_springsEndIndexTex, int(springIndex), uvec4(endIndex));
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
		imageStore(u_massPointsPositionTex, currentIndex, vec4(pos, 1.0));
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
