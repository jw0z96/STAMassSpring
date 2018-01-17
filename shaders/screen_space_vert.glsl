#version 430

// The vertex position attribute
layout (location=0) in vec3 VertexPosition;

// We need an MVP because the plane needs to be rotated
uniform mat4 SSMVP;

void main()
{
	// Set the position of the current vertex
	gl_Position = SSMVP * vec4(VertexPosition, 1.0);
}

