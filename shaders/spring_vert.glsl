#version 430 core

uniform mat4 MVP;

flat out uint springType;

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
	uint type;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	Mass masses[];
};

layout (std430, binding = 1) buffer springsBuffer
{
	Spring springs[];
};

out float strain;

void main()
{
	int springId = int(floor((gl_VertexID / 2.0)));

	springType = springs[springId].type;
	uint startIndex = springs[springId].start;
	vec3 startPos = masses[startIndex].position.xyz;
	uint endIndex = springs[springId].end;
	vec3 endPos = masses[endIndex].position.xyz;
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
