#version 430 core

// The output textures that make up our gBuffer
layout (location=0) out vec3 fragWSPosition;
layout (location=1) out vec3 fragWSNormal;
layout (location=2) out vec3 fragAlbedo;
layout (location=3) out vec2 fragMetalRough;

in float strain;

flat in uint springType;

const uint SPRING_TYPE_STRUCTURAL = 0;
const uint SPRING_TYPE_BEND = 1;
const uint SPRING_TYPE_SHEAR = 2;

uniform bool u_drawStructuralSprings;
uniform bool u_drawBendSprings;
uniform bool u_drawShearSprings;

void main()
{
	vec3 diffuse = vec3(0.0);
	if ((u_drawStructuralSprings && springType == SPRING_TYPE_STRUCTURAL) ||
		(u_drawShearSprings && springType == SPRING_TYPE_SHEAR) ||
		(u_drawBendSprings && springType == SPRING_TYPE_BEND))
	{
		diffuse = vec3(max(100.0 * strain, 0.0), 0.0, max(-100.0 * strain, 0.0));
	}
	else
	{
		discard;
	}
	fragAlbedo = vec3(diffuse);
	fragWSPosition = vec3(0.0);
	fragWSNormal = vec3(0.0);
	fragMetalRough = vec2(0.0, 0.0);
}
