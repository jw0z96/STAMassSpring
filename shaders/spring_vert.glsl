#version 430 core

uniform mat4 MVP;

struct State
{
	vec4 position;
	vec4 velocity;
};

struct Spring
{
	State state;
	uint start;
	uint end;
	float restingLength;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	State masses[];
};

layout (std430, binding = 1) buffer springsBuffer
{
	Spring springs[];
};

out float strain;

void main()
{
	int springId = int(floor((gl_VertexID / 2.0)));

	// uint startIndex = imageLoad(u_springsStartIndexTex, springId).x;
	uint startIndex = springs[springId].start;
	// vec3 startPos = texelFetch(massPointsPositionTex, int(startIndex), 0).xyz;
	vec3 startPos = masses[startIndex].position.xyz;
	// uint endIndex = imageLoad(u_springsEndIndexTex, springId).x;
	uint endIndex = springs[springId].end;
	// vec3 endPos = texelFetch(massPointsPositionTex, int(endIndex), 0).xyz;
	vec3 endPos = masses[endIndex].position.xyz;
	// float restingLength = imageLoad(u_springsRestingLengthTex, springId).x;
	float restingLength = springs[springId].restingLength;

	strain = distance(startPos, endPos) - restingLength;

	// even index
	if(mod(float(gl_VertexID), 2.0) == 0.0)
	{
		gl_Position = MVP * vec4(startPos, 1.0);
	}
	else // odd index
	{
		gl_Position = MVP * vec4(endPos, 1.0);
	}

}