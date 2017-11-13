#include "Spring.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

Spring::Spring(std::shared_ptr<ngl::Vec3> _s, std::shared_ptr<ngl::Vec3> _e) :
	m_startPoint(_s),
	m_endPoint(_e)
{
	std::cout<<"constructed spring\n";
}

Spring::~Spring()
{
	std::cout<<"destructed spring\n";
}
