#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (binding = 0, rgba32f) uniform image1D u_massPointsPositionTex;
layout (binding = 1, r16f) uniform readonly image1D u_springsRestingLengthTex;
layout (binding = 2, rgba8) uniform image1D u_springsStatePositionTex;
layout (binding = 3, rgba8) uniform image1D u_springsStateVelocityTex;
layout (binding = 4, r16ui) coherent uniform uimage1D u_springsStartIndexTex;
layout (binding = 5, r16ui) coherent uniform uimage1D u_springsEndIndexTex;

uniform float u_currentTime;
uniform float u_timeStep;

void main()
{
	uint computeIndex = gl_GlobalInvocationID.x;

	// computeIndex = 10;

	uint startIndex = imageLoad(u_springsStartIndexTex, int(computeIndex)).x;
	uint endIndex = imageLoad(u_springsEndIndexTex, int(computeIndex)).x;

	vec3 diff = vec3(0.0, sin(u_currentTime), 0.0);

	vec3 currentPos = imageLoad(u_massPointsPositionTex, int(startIndex)).xyz;
	currentPos += 0.001*diff;
	imageStore(u_massPointsPositionTex, int(startIndex), vec4(currentPos, 1.0));

	currentPos = imageLoad(u_massPointsPositionTex, int(endIndex)).xyz;
	currentPos += 0.001*diff;
	imageStore(u_massPointsPositionTex, int(endIndex), vec4(currentPos, 1.0));
}
