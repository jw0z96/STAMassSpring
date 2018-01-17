#version 430 core

// The output textures that make up our gBuffer
layout (location=0) out vec3 fragWSPosition;
layout (location=1) out vec3 fragWSNormal;
layout (location=2) out vec3 fragAlbedo;
layout (location=3) out vec2 fragMetalRough;

flat in int axis;

smooth in vec3 f_normal;
smooth in vec3 f_pos;

uniform vec3 camPos;

void main()
{
	vec3 diffuse = vec3(0.0);

	if (axis == 0)
		diffuse = vec3(1.0, 0.0, 0.0);
	else if (axis == 1)
		diffuse = vec3(0.0, 1.0, 0.0);
	else if (axis == 2)
		diffuse = vec3(0.0, 0.0, 1.0);
	else if (axis == 3)
		diffuse = vec3(1.0, 1.0, 0.0);
	else if (axis == 4)
		diffuse = vec3(1.0, 0.0, 1.0);
	else if (axis == 5)
		diffuse = vec3(1.0, 1.0, 1.0);

	fragAlbedo = vec3(diffuse);
	fragWSPosition = f_pos;
	fragWSNormal = f_normal;
	fragMetalRough = vec2(0.0, 0.0);
	// fragColor = vec4(f_pos, 0.0);
}
