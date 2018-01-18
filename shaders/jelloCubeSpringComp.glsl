#version 430

#extension GL_NV_shader_atomic_float : enable

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// TOO LAZY TO PAD MANUALLY, REMEMBER TO ONLY ACCESS .XYZ
struct Mass
{
	vec4 position;
	vec4 velocity;
	vec4 force;
};

struct Spring
{
	uint start;
	uint end;
	float restingLength;
	vec4 velocity;
	uint type;
};

layout (std430, binding = 0) buffer massPointsBuffer
{
	Mass masses[];
};

layout (std430, binding = 1) buffer springsBuffer
{
	Spring springs[];
};

const uint INTEGRATOR_TYPE_RK4 = 0;
const uint INTEGRATOR_TYPE_RK2 = 1;
const uint INTEGRATOR_TYPE_EULER = 2;

uniform float u_currentTime;
uniform float u_timeStep;

uniform int u_integrator;
uniform float u_k;
uniform float u_damping;
uniform float u_recovery;

uniform bool u_writeMode;
uniform int u_springCount;

uniform int u_sizeX;
uniform int u_sizeY;
uniform int u_sizeZ;

// ----------------------------------------------------------------------------
/// The following section is modified from:
/// Jon Macey (2015): Mass Spring System using RK 4 integration
/// Available from https://github.com/NCCA/MassSpring
// ----------------------------------------------------------------------------
// return the velocity (or force??) calculated by the spring according to hooke's law
// vec3 _v: the relative velocity between the two masses
/// F = -k(|x|-d)(x/|x|) - bv where
/// k is the spring constant
/// |x| is the distance between the two spring points
/// d is the distance of seperation
/// b is the coefficient of damping larger b increases the damping force
/// v is the relative velocity between the spring points
vec3 motionFunction(vec3 _v)
{
	uint computeIndex = gl_GlobalInvocationID.x;
	uint startIndex = springs[computeIndex].start;
	uint endIndex = springs[computeIndex].end;
	vec3 startPos = masses[startIndex].position.xyz;
	vec3 endPos = masses[endIndex].position.xyz;
	vec3 distance = endPos - startPos;
	float length = length(distance);
	springs[computeIndex].restingLength = mix(springs[computeIndex].restingLength, length, ((1.0 - u_recovery) * u_timeStep));
	float restingLength = springs[computeIndex].restingLength;
	return -u_k*(length-restingLength)*(distance/length)-u_damping*_v;
}
// ----------------------------------------------------------------------------
// calculate the velocity of the spring
// vec3 _v: the relative velocity between masses
vec3 evaluate(vec3 _v)
{
	return motionFunction(_v);
}
// ----------------------------------------------------------------------------
// calculate the velocity of the spring
// vec3 _v: the relative velocity between masses
vec3 evaluate(vec3 _v, float _dt, vec3 _dv)
{
	vec3 v2 = _v + _dv * _dt;
	return motionFunction(v2);
}
// ----------------------------------------------------------------------------
// perform RK4 integration and return dv, change in velocity
// vec3 _v: the current relative velocity between masses
vec3 integrateRK4(vec3 _v)
{
	vec3 a = evaluate(_v);
	vec3 b = evaluate(_v, 0.5 * u_timeStep, a);
	vec3 c = evaluate(_v, 0.5 * u_timeStep, b);
	vec3 d = evaluate(_v, u_timeStep, c);

	vec3 dvdt = 1.0f/6.0f * (a + 2.0 * (b + c) + d);
	return dvdt * u_timeStep;
}
/// end of citation
// ----------------------------------------------------------------------------
// perform RK2 integration and return dv, change in velocity
// vec3 _v: the current relative velocity between masses
vec3 integrateRK2(vec3 _v)
{
	vec3 a = evaluate(_v);
	vec3 b = evaluate(_v, 0.5 * u_timeStep, a);
	vec3 dvdt = 0.5 * (a + b);
	return dvdt * u_timeStep;
}
// ----------------------------------------------------------------------------
// perform Euler integration and return dv, change in velocity
// vec3 _v: the current relative velocity between masses
vec3 integrateEuler(vec3 _v)
{
	vec3 dvdt = evaluate(_v);
	return dvdt * u_timeStep;
}
// ----------------------------------------------------------------------------
void main()
{
	uint computeIndex = gl_GlobalInvocationID.x;

	if (computeIndex >= u_springCount)
	return;

	if (!u_writeMode)
	{
		if (u_integrator == INTEGRATOR_TYPE_RK4)
		{
			springs[computeIndex].velocity.xyz = integrateRK4(springs[computeIndex].velocity.xyz);
		}
		else if (u_integrator == INTEGRATOR_TYPE_RK2)
		{
			springs[computeIndex].velocity.xyz = integrateRK2(springs[computeIndex].velocity.xyz);
		}
		else if (u_integrator == INTEGRATOR_TYPE_EULER)
		{
			springs[computeIndex].velocity.xyz = integrateEuler(springs[computeIndex].velocity.xyz);
		}
	}
	else
	{
		uint startIndex = springs[computeIndex].start;
		uint endIndex = springs[computeIndex].end;

		// get force from integration result (dv)
		vec3 springForce = (springs[computeIndex].velocity.xyz / u_timeStep) * 10.0; // * u_mass;

		atomicAdd(masses[startIndex].force.x, -springForce.x);
		atomicAdd(masses[startIndex].force.y, -springForce.y);
		atomicAdd(masses[startIndex].force.z, -springForce.z);

		atomicAdd(masses[endIndex].force.x, springForce.x);
		atomicAdd(masses[endIndex].force.y, springForce.y);
		atomicAdd(masses[endIndex].force.z, springForce.z);
	}
}
