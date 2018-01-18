#include "JelloCube.h"

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

JelloCube::JelloCube(double _k, double _damping) : m_k(_k), m_damping(_damping)
{
	std::cout<<"creating jello cube!\n";
	m_timestep = 0.01;
	m_subSteps = 10;
	m_t = 0.0;
	m_sizeX = 10;
	m_sizeY = 10;
	m_sizeZ = 10;
	m_mass = 1.0;
	m_gravity = -9.81;
	m_friction = 0.2;
	m_recovery = 1.0;
	m_integrator = 0;
	m_enableCollider = true;
	m_numQuads = 0;
	m_massBufferId = 0;
	m_springBufferId = 0;
	m_simulateToggle = true;
}

JelloCube::~JelloCube()
{
	std::cout<<"bye!\n";
	glDeleteBuffers(1, &m_massBufferId);
	glDeleteBuffers(1, &m_springBufferId);
	glDeleteVertexArrays(1, &m_emptyVAO);
}

void JelloCube::reset()
{
	m_t = 0.0;
	generate();
}

void JelloCube::initializeShaders()
{
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();

	glEnable(GL_NV_shader_atomic_float);

	// create the compute shader program for initializing the textures (setting up springs & mass positions)
	shader->createShaderProgram("jelloCubeInitPass");
	shader->attachShader("jelloCubeInitPassComp", ngl::ShaderType::COMPUTE );
	shader->loadShaderSource("jelloCubeInitPassComp", "shaders/jelloCubeInitComp.glsl" );
	shader->compileShader("jelloCubeInitPassComp");
	shader->attachShaderToProgram("jelloCubeInitPass", "jelloCubeInitPassComp");
	shader->linkProgramObject("jelloCubeInitPass");

	// create the compute shader program for initializing the textures (setting up springs & mass positions)
	shader->createShaderProgram("jelloCubeSpringPass");
	shader->attachShader("jelloCubeSpringPassComp", ngl::ShaderType::COMPUTE );
	shader->loadShaderSource("jelloCubeSpringPassComp", "shaders/jelloCubeSpringComp.glsl" );
	shader->compileShader("jelloCubeSpringPassComp");
	shader->attachShaderToProgram("jelloCubeSpringPass", "jelloCubeSpringPassComp");
	shader->linkProgramObject("jelloCubeSpringPass");

	// create the compute shader program for calculating the external forces in the simulation
	shader->createShaderProgram("jelloCubeExternalForcesPass");
	shader->attachShader("jelloCubeExternalForcesPassComp", ngl::ShaderType::COMPUTE );
	shader->loadShaderSource("jelloCubeExternalForcesPassComp", "shaders/jelloCubeExternalForcesPassComp.glsl" );
	shader->compileShader("jelloCubeExternalForcesPassComp");
	shader->attachShaderToProgram("jelloCubeExternalForcesPass", "jelloCubeExternalForcesPassComp");
	shader->linkProgramObject("jelloCubeExternalForcesPass");

	// create the spring geometry shader program
	shader->loadShader("springShader",
		"shaders/spring_vert.glsl",
		"shaders/spring_frag.glsl");
	shader->use("springShader");

	// create the spring geometry shader program
	shader->loadShader("jelloGeoShader",
		"shaders/jello_vert.glsl",
		"shaders/jello_frag.glsl",
		"shaders/jello_geo.glsl");
	shader->use("jelloGeoShader");

	generate();

	// create empty vao for procedural geometry
	glGenVertexArrays(1, &m_emptyVAO); // Create our Vertex Array Object
	glBindVertexArray(m_emptyVAO); // Bind our Vertex Array Object so we can use it
	glBindVertexArray(0);
}

void JelloCube::generate()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeInitPass");

	ngl::Vec3 topRight = ngl::Vec3(3.0, 6.0, 3.0);
	ngl::Vec3 bottomLeft = ngl::Vec3(0.0, 3.0, 0.0);
	ngl::Vec3 span = topRight - bottomLeft;
	ngl::Vec3 step = span / ngl::Vec3(m_sizeX, m_sizeY, m_sizeZ);

	shader->setUniform("u_bottomLeft", bottomLeft);
	shader->setUniform("u_step", step);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	// create an atomic counter to get the number of required springs
	GLuint springCounter;
	genAtomicCounter(springCounter);
	// bind the atomic counter
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, springCounter);

	// calculate the number of quads needed to draw the mesh
	m_numQuads = (m_sizeX - 1) * (m_sizeY - 1) * 2;
	m_numQuads += (m_sizeX - 1) * (m_sizeZ - 1) * 2;
	m_numQuads += (m_sizeY - 1) * (m_sizeZ - 1) * 2;

	// create the texture to store the positions of the mass points
	m_massCount = m_sizeX * m_sizeY * m_sizeZ;
	std::vector<SSBO_Mass> massPointsArray(m_massCount);

	// generate an ssbo for the masses
	genSSBO(m_massBufferId, m_massCount * sizeof(SSBO_Mass), &massPointsArray[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);

	// set to only count the number of springs
	shader->setUniform("u_springWrite", false);
	// dispatch compute shader
	glDispatchCompute(m_sizeX, m_sizeY, m_sizeZ);

	// read the atomic counter value
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, springCounter);
	GLuint *count;
	count = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
		0,
		sizeof(GLuint),
		GL_MAP_READ_BIT);
	std::cout<<"spring count: "<<count[0]<<"\n";

	m_springCount = count[0];

	// reset the counter to 0
	memset(count, 0, sizeof(GLuint));
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// generate an ssbo for the springs
	std::vector<SSBO_Spring> springsArray(m_springCount);
	genSSBO(m_springBufferId, m_springCount * sizeof(SSBO_Spring), &springsArray[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_springBufferId);

	// set to write the contents of springs
	shader->setUniform("u_springWrite", true);
	// dispatch compute shader (this time to store springs)
	glDispatchCompute(m_sizeX, m_sizeY, m_sizeZ);

	// read the atomic counter value
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, springCounter);
	*count;
	count = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
		0,
		sizeof(GLuint),
		GL_MAP_READ_BIT);
	std::cout<<"spring count (the second time around): "<<count[0]<<"\n";
	m_springCount = count[0];
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// delete the atomic counter
	glDeleteBuffers(1, &springCounter);
}

void JelloCube::update(ngl::Vec3 _pos, float _radius)
{
	if (m_simulateToggle)
	{
		for (int i = 0; i < m_subSteps; ++i)
		{
			calculateSpringForces();
			calculateExternalForces(_pos, _radius);
			// update the timestep for the next time
			m_t += m_timestep / (float)m_subSteps;
		}
	}
	else
	{
		return;
	}
}

void JelloCube::calculateSpringForces()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeSpringPass");

	shader->setUniform("u_currentTime", m_t);
	shader->setUniform("u_timeStep", float(m_timestep / (float)m_subSteps));
	shader->setUniform("u_k", m_k);
	shader->setUniform("u_damping", m_damping);
	shader->setUniform("u_recovery", m_recovery);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	shader->setUniform("u_integrator", GLint(m_integrator));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_springBufferId);

	shader->setUniform("u_springCount", GLint(m_springCount));
	shader->setUniform("u_writeMode", false);

	// dispatch compute shader to integrate springs
	glDispatchCompute(m_springCount, 1, 1);

	// dispatch compute shader to update springs
	shader->setUniform("u_writeMode", true);
	glDispatchCompute(m_springCount, 1, 1);
}

void JelloCube::calculateExternalForces(ngl::Vec3 _pos, float _radius)
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeExternalForcesPass");

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	shader->setUniform("u_currentTime", m_t);
	shader->setUniform("u_timeStep", float(m_timestep / (float)m_subSteps));

	shader->setUniform("u_mass", GLfloat(m_mass / (float)1000.0));
	// shader->setUniform("u_mass", GLfloat(m_mass / (float)m_massCount));
	shader->setUniform("u_gravity", m_gravity);
	shader->setUniform("u_friction", m_friction);

	shader->setUniform("u_sphereCollisions", m_enableCollider);
	shader->setUniform("u_spherePos", _pos);
	shader->setUniform("u_sphereRadius", _radius);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);
	glDispatchCompute(m_sizeX, m_sizeY, m_sizeZ);
}
