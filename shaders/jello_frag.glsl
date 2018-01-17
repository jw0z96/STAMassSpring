#version 430 core

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

flat in int axis;

smooth in vec3 f_normal;
smooth in vec3 f_pos;

uniform vec3 camPos;

void main()
{
	vec3 lightPos = vec3(10.0);
	vec3 L = normalize(lightPos - f_pos);
	float NdotL = max(0.0, dot(L, f_normal));
	float LdotV = max(0.0, dot(normalize(camPos - f_pos), reflect(f_normal, L)));
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

	fragColor = vec4(diffuse * (LdotV + NdotL), 0.0);
	// fragColor = vec4(f_pos, 0.0);
}
