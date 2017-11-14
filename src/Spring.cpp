#include "Spring.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

Spring::Spring(std::shared_ptr<ngl::Vec3> _s, std::shared_ptr<ngl::Vec3> _e) :
	m_startPoint(_s),
	m_endPoint(_e)
{
	// set the spring constant and damping coefficient
	m_k = 2.0;
	m_damping = 1.0;

	// set resting length
	ngl::Vec3 distance = *(m_endPoint) - *(m_startPoint);
	// m_restingLength = distance.length();
	m_restingLength = 0.5;
	std::cout<<"constructed spring of length "<<m_restingLength<<"\n";

	m_state.m_position = distance;
	m_state.m_velocity = ngl::Vec3(0.0, 0.0, 0.0);
}

Spring::~Spring()
{
	std::cout<<"destructed spring\n";
}

void Spring::update()
{
	*(m_startPoint) -= m_state.m_velocity;
	*(m_endPoint) += m_state.m_velocity;
}

//----------------------------------------------------------------------------------------------------------------------

State Spring::evaluate(const State &_initial, float _t)
{

	State output;
	output.m_position=_initial.m_velocity;
	output.m_velocity=motionFunction(_initial, _t);
	return output;
}

//----------------------------------------------------------------------------------------------------------------------

State Spring::evaluate(const State &_initial, float _t,float _dt, const State &_d)
{

	State state;
	state.m_position = _initial.m_position + _d.m_position*_dt;
	state.m_velocity = _initial.m_velocity + _d.m_velocity*_dt;
	State output;
	output.m_position = state.m_velocity;
	output.m_velocity = motionFunction(state, _t+_dt);
	return output;
}

//----------------------------------------------------------------------------------------------------------------------

void Spring::integrate(float _t, float _dt)
{
	// basic RK4 integration we calculate the initial value then two slopes
	// and combine together see
	// http://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods for more details
	State a = evaluate(m_state, _t);
	State b = evaluate(m_state, _t, _dt*0.5f, a);
	State c = evaluate(m_state, _t, _dt*0.5f, b);
	State d = evaluate(m_state, _t, _dt, c);
	const ngl::Vec3 dxdt = 1.0f/6.0f * (a.m_position + 2.0f*(b.m_position + c.m_position) + d.m_position);
	const ngl::Vec3 dvdt = 1.0f/6.0f * (a.m_velocity + 2.0f*(b.m_velocity + c.m_velocity) + d.m_velocity);

	m_state.m_position = m_state.m_position + dxdt*_dt;
	m_state.m_velocity = m_state.m_velocity + dvdt*_dt;
}

//----------------------------------------------------------------------------------------------------------------------

ngl::Vec3 Spring::motionFunction(const State &_state, float _t)
{
	// in the case of the spring we don't need t mark
	// it as unused in this case
	/// F = -k(|x|-d)(x/|x|) - bv where
	/// k is the spring constant
	/// |x| is the distance between the two spring points
	/// d is the distance of seperation
	/// b is the coefficient of damping larger b increases the damping force
	/// v is the relative velocity between the spring points
	ngl::Vec3 distance = *(m_endPoint) - *(m_startPoint);
	float length=distance.length();
	return -m_k*(length-m_restingLength)*(distance/length)-m_damping*_state.m_velocity;
}
