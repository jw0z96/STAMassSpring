#version 430 core

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

flat in int axis;

void main()
{
	if (axis == 0)
		fragColor = vec4(1.0, 0.0, 0.0, 0.0);
	else if (axis == 1)
		fragColor = vec4(0.0, 1.0, 0.0, 0.0);
	else if (axis == 2)
		fragColor = vec4(0.0, 0.0, 1.0, 0.0);
	else if (axis == 3)
		fragColor = vec4(1.0, 1.0, 0.0, 0.0);
	else if (axis == 4)
		fragColor = vec4(1.0, 0.0, 1.0, 0.0);
	else if (axis == 5)
		fragColor = vec4(1.0, 1.0, 1.0, 0.0);
	else
		fragColor = vec4(0.0);
}
