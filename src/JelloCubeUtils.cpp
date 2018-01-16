#include "JelloCube.h"

void JelloCube::setK(double _k)
{
	m_k = _k;
}

void JelloCube::setDamping(double _damping)
{
	m_damping = _damping;
}

void JelloCube::setMass(double _mass)
{
	m_mass = _mass;
}

void JelloCube::setGravity(double _gravity)
{
	m_gravity = _gravity;
}

void JelloCube::setTimeStep(double _t)
{
	m_timestep = _t;
}

void JelloCube::setSubSteps(int _s)
{
	m_subSteps = _s;
}