#version 430

#extension GL_NV_shader_atomic_float : enable

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct State
{
	vec4 position;
	vec4 velocity;
};

struct Spring
{
	State state;
	uint start;
	uint end;
	float restingLength;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	State masses[];
};

layout (std430, binding = 1) buffer springsBuffer
{
	Spring springs[];
};

uniform float u_currentTime;
uniform float u_timeStep;

uniform float u_k;
uniform float u_damping;

uniform bool u_writeMode;
uniform int u_springCount;

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

// return the velocity (or force??) calculated by the spring according to hooke's law
// vec3 _v: the relative velocity between the two masses
vec3 motionFunction(vec3 _v)
{
	uint computeIndex = gl_GlobalInvocationID.x;
	uint startIndex = springs[computeIndex].start;
	uint endIndex = springs[computeIndex].end;
	vec3 startPos = masses[startIndex].position.xyz;
	vec3 endPos = masses[endIndex].position.xyz;
	vec3 distance = endPos - startPos;
	float length = length(distance);
	float restingLength = springs[computeIndex].restingLength;
	return -u_k * (length-restingLength) * (distance/length) -u_damping * _v;
}

// calculate the velocity of the spring
vec3 evaluate(vec3 _v)
{
	return motionFunction(_v);
}

// calculate the velocity of the spring
vec3 evaluate(vec3 _v, float _dt, vec3 _dv)
{
	vec3 v2 = _v + _dv * _dt;
	return motionFunction(v2);
}

State integrate(State _currentState)
{
	vec3 a = evaluate(_currentState.velocity.xyz);
	vec3 b = evaluate(_currentState.velocity.xyz, 0.5 * u_timeStep, a);
	vec3 c = evaluate(_currentState.velocity.xyz, 0.5 * u_timeStep, b);
	vec3 d = evaluate(_currentState.velocity.xyz, u_timeStep, c);

	vec3 dvdt = 1.0f/6.0f * (a + 2.0 * (b + c) + d);

	State newState;
	newState.position.xyz = vec3(0.0);
	newState.velocity.xyz = dvdt * u_timeStep;
	return newState;
}

// void atomicAddVec3(inout vec3 location, vec3 data)
// {
// 	atomicAdd(location.x, data.x);
// 	atomicAdd(location.y, data.y);
// 	atomicAdd(location.z, data.z);
// }

void main()
{
	uint computeIndex = gl_GlobalInvocationID.x;

	if (computeIndex >= u_springCount)
		return;

	if (!u_writeMode)
	{
		State newState = integrate(springs[computeIndex].state);
		springs[computeIndex].state = newState;
	}
	else
	{
		uint startIndex = springs[computeIndex].start;
		uint endIndex = springs[computeIndex].end;

		// if (startIndex != (u_sizeX * u_sizeY * u_sizeZ) - 1 &&
		// 	startIndex != (u_sizeX * u_sizeY * u_sizeZ) - u_sizeZ &&
		// 	startIndex != (u_sizeX * u_sizeY) - 1 &&
		// 	startIndex != (u_sizeX * u_sizeY) - u_sizeX)
		// {
			atomicAdd(masses[startIndex].velocity.x, -springs[computeIndex].state.velocity.x);
			atomicAdd(masses[startIndex].velocity.y, -springs[computeIndex].state.velocity.y);
			atomicAdd(masses[startIndex].velocity.z, -springs[computeIndex].state.velocity.z);
		// }

		// if (endIndex != (u_sizeX * u_sizeY * u_sizeZ) - 1)
		// if (endIndex != (u_sizeX * u_sizeY * u_sizeZ) - 1 &&
		// 	endIndex != (u_sizeX * u_sizeY * u_sizeZ) - u_sizeZ &&
		// 	endIndex != (u_sizeX * u_sizeY) - 1 &&
		// 	endIndex != (u_sizeX * u_sizeY) - u_sizeX)
		// {
			atomicAdd(masses[endIndex].velocity.x, springs[computeIndex].state.velocity.x);
			atomicAdd(masses[endIndex].velocity.y, springs[computeIndex].state.velocity.y);
			atomicAdd(masses[endIndex].velocity.z, springs[computeIndex].state.velocity.z);
		// }
	}
}
