#version 430 core

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
