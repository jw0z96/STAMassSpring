#version 430 core

uniform mat4 MVP;

uniform sampler1D massPointsPositionTex;

layout (binding = 0, r16ui) coherent uniform readonly uimage1D u_springsStartIndexTex;
layout (binding = 1, r16ui) coherent uniform readonly uimage1D u_springsEndIndexTex;
layout (binding = 2, r32f) uniform readonly image1D u_springsRestingLengthTex;

out float strain;

void main()
{
	int springId = int(floor((gl_VertexID / 2.0)));

	uint startIndex = imageLoad(u_springsStartIndexTex, springId).x;
	vec3 startPos = texelFetch(massPointsPositionTex, int(startIndex), 0).xyz;
	uint endIndex = imageLoad(u_springsEndIndexTex, springId).x;
	vec3 endPos = texelFetch(massPointsPositionTex, int(endIndex), 0).xyz;
	float restingLength = imageLoad(u_springsRestingLengthTex, springId).x;

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
