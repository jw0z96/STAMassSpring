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

vec3 motionFunction(State _state, float _t)
{
	uint computeIndex = gl_GlobalInvocationID.x;
	uint startIndex = springs[computeIndex].start;
	uint endIndex = springs[computeIndex].end;
	vec3 startPos = masses[startIndex].position.xyz;
	vec3 endPos = masses[endIndex].position.xyz;
	vec3 distance = endPos - startPos;
	float length = length(distance);
	float restingLength = springs[computeIndex].restingLength;
	return -u_k*(length-restingLength)*(distance/length)-u_damping*_state.velocity.xyz;
}

State evaluate(State _state, float _t)
{
	State outputState;
	outputState.position.xyz = _state.velocity.xyz;
	outputState.velocity.xyz = motionFunction(_state, _t);
	return outputState;
}

State evaluate(State _state, float _t, float _dt, State _d)
{
	State state1;
	state1.position.xyz = _state.position.xyz + _d.position.xyz * _dt;
	state1.velocity.xyz = _state.velocity.xyz + _d.velocity.xyz * _dt;
	State outputState;
	outputState.position.xyz = _state.velocity.xyz;
	outputState.velocity.xyz = motionFunction(state1, _t + _dt);
	return outputState;
}

State integrate(State _currentState)
{
	State a = evaluate(_currentState, u_currentTime);
	State b = evaluate(_currentState, u_currentTime, 0.5 * u_timeStep, a);
	State c = evaluate(_currentState, u_currentTime, 0.5 * u_timeStep, b);
	State d = evaluate(_currentState, u_currentTime, u_timeStep, c);

	vec3 dxdt = 1.0f/6.0f * (a.position.xyz + 2.0 * (b.position.xyz + c.position.xyz) + d.position.xyz);
	vec3 dvdt = 1.0f/6.0f * (a.velocity.xyz + 2.0 * (b.velocity.xyz + c.velocity.xyz) + d.velocity.xyz);

	State newState;
	newState.position.xyz = _currentState.position.xyz + dxdt * u_timeStep;
	newState.velocity.xyz = _currentState.velocity.xyz + dvdt * u_timeStep;
	return newState;
}

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

		atomicAdd(masses[startIndex].position.x, -springs[computeIndex].state.velocity.x);
		atomicAdd(masses[startIndex].position.y, -springs[computeIndex].state.velocity.y);
		atomicAdd(masses[startIndex].position.z, -springs[computeIndex].state.velocity.z);

		atomicAdd(masses[endIndex].position.x, springs[computeIndex].state.velocity.x);
		atomicAdd(masses[endIndex].position.y, springs[computeIndex].state.velocity.y);
		atomicAdd(masses[endIndex].position.z, springs[computeIndex].state.velocity.z);
	}
}
