#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (binding = 0, rgba32f) uniform image1D u_massPointsPositionTex;
layout (binding = 1, r32f) uniform readonly image1D u_springsRestingLengthTex;
layout (binding = 2, rgba32f) uniform image1D u_springsStatePositionTex;
layout (binding = 3, rgba32f) uniform image1D u_springsStateVelocityTex;
layout (binding = 4, r16ui) coherent uniform readonly uimage1D u_springsStartIndexTex;
layout (binding = 5, r16ui) coherent uniform readonly uimage1D u_springsEndIndexTex;

uniform float u_currentTime;
uniform float u_timeStep;

uniform float u_k;
uniform float u_damping;

uniform bool u_writeMode;
uniform int u_springCount;

struct State
{
	vec3 position;
	vec3 velocity;
};

vec3 motionFunction(State _state, float _t)
{
	uint computeIndex = gl_GlobalInvocationID.x;
	uint startIndex = imageLoad(u_springsStartIndexTex, int(computeIndex)).x;
	uint endIndex = imageLoad(u_springsEndIndexTex, int(computeIndex)).x;
	vec3 startPos = imageLoad(u_massPointsPositionTex, int(startIndex)).xyz;
	vec3 endPos = imageLoad(u_massPointsPositionTex, int(endIndex)).xyz;
	vec3 distance = endPos - startPos;
	float length = length(distance);
	float restingLength = imageLoad(u_springsRestingLengthTex, int(computeIndex)).x;
	return -u_k*(length-restingLength)*(distance/length)-u_damping*_state.velocity;
}

State evaluate(State _state, float _t)
{
	State outputState;
	outputState.position = _state.velocity;
	outputState.velocity = motionFunction(_state, _t);
	return outputState;
}

State evaluate(State _state, float _t, float _dt, State _d)
{
	State state1;
	state1.position = _state.position + _d.position * _dt;
	state1.velocity = _state.velocity + _d.velocity * _dt;
	State outputState;
	outputState.position = _state.velocity;
	outputState.velocity = motionFunction(state1, _t + _dt);
	return outputState;
}

State integrate(State _currentState)
{
	State a = evaluate(_currentState, u_currentTime);
	State b = evaluate(_currentState, u_currentTime, 0.5 * u_timeStep, a);
	State c = evaluate(_currentState, u_currentTime, 0.5 * u_timeStep, b);
	State d = evaluate(_currentState, u_currentTime, u_timeStep, c);

	vec3 dxdt = 1.0f/6.0f * (a.position + 2.0 * (b.position + c.position) + d.position);
	vec3 dvdt = 1.0f/6.0f * (a.velocity + 2.0 * (b.velocity + c.velocity) + d.velocity);

	State newState;
	newState.position = _currentState.position + dxdt * u_timeStep;
	newState.velocity = _currentState.velocity + dvdt * u_timeStep;
	return newState;
}

void main()
{
	uint computeIndex = gl_GlobalInvocationID.x;

	if (!u_writeMode)
	{
		uint startIndex = imageLoad(u_springsStartIndexTex, int(computeIndex)).x;
		uint endIndex = imageLoad(u_springsEndIndexTex, int(computeIndex)).x;

		vec3 statePosition = imageLoad(u_springsStatePositionTex, int(computeIndex)).xyz;
		vec3 stateVelocity = imageLoad(u_springsStateVelocityTex, int(computeIndex)).xyz;
		State currentState = State(statePosition, stateVelocity);

		State newState = integrate(currentState);

		imageStore(u_springsStatePositionTex, int(computeIndex), vec4(newState.position, 1.0));
		imageStore(u_springsStateVelocityTex, int(computeIndex), vec4(newState.velocity, 1.0));
	}
	else
	{
		for (uint i = 0; i < u_springCount; ++i)
		{
			uint startIndex = imageLoad(u_springsStartIndexTex, int(i)).x;
			uint endIndex = imageLoad(u_springsEndIndexTex, int(i)).x;
			vec3 stateVelocity = imageLoad(u_springsStateVelocityTex, int(i)).xyz;

			vec3 startPos = imageLoad(u_massPointsPositionTex, int(startIndex)).xyz;
			imageStore(u_massPointsPositionTex, int(startIndex), vec4(startPos - stateVelocity, 1.0));

			vec3 endPos = imageLoad(u_massPointsPositionTex, int(endIndex)).xyz;
			imageStore(u_massPointsPositionTex, int(endIndex), vec4(endPos + stateVelocity, 1.0));
		}
	}
}
