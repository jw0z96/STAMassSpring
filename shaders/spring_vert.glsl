#version 430 core

uniform mat4 MVP;

uniform sampler1D massPointsPositionTex;
// uniform sampler1D springStartIndexTex;
// uniform sampler1D springEndIndexTex;

layout (binding = 0, r16ui) coherent uniform readonly uimage1D u_springsStartIndexTex;
layout (binding = 1, r16ui) coherent uniform readonly uimage1D u_springsEndIndexTex;

void main()
{
	int springId = int(floor((gl_VertexID / 2.0)));

	// even index
	if(mod(float(gl_VertexID), 2.0) == 0.0)
	{
		uint startIndex = imageLoad(u_springsStartIndexTex, springId).x;
		vec3 startPos = texelFetch(massPointsPositionTex, int(startIndex), 0).xyz;
		gl_Position = MVP * vec4(startPos, 1.0);
	}
	else // odd index
	{
		uint endIndex = imageLoad(u_springsEndIndexTex, springId).x;
		vec3 endPos = texelFetch(massPointsPositionTex, int(endIndex), 0).xyz;
		gl_Position = MVP * vec4(endPos, 1.0);
	}

}
