#version 430 core

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

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

	if ((u_drawStructuralSprings && springType == SPRING_TYPE_STRUCTURAL) ||
		(u_drawShearSprings && springType == SPRING_TYPE_SHEAR) ||
		(u_drawBendSprings && springType == SPRING_TYPE_BEND))
	{
		fragColor = vec4(max(100.0 * strain, 0.0), 0.0, max(-100.0 * strain, 0.0), 1.0);
	}
	else
	{
		discard;
	}
}
