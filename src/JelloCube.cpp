#include "JelloCube.h"

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------

JelloCube::JelloCube(double _k, double _damping) : m_k(_k), m_damping(_damping)
{
	std::cout<<"creating jello cube!\n";
	m_timestep = 0.1;
	m_t = 0.0;
	m_sizeX = 50;
	m_sizeY = 50;
	m_sizeZ = 50;
}

JelloCube::~JelloCube()
{
	std::cout<<"bye!\n";
	glDeleteBuffers(1, &m_massBufferId);
	glDeleteBuffers(1, &m_springBufferId);
	glDeleteVertexArrays(1, &m_emptyVAO);
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

	// create the spring geometry shader program
	shader->loadShader("springShader",
		"shaders/spring_vert.glsl",
		"shaders/spring_frag.glsl");
	shader->use("springShader");
	shader->setUniform("massPointsPositionTex", 0);

	generate();

	// create empty vao for procedural geometry
	glGenVertexArrays(1, &m_emptyVAO); // Create our Vertex Array Object
	glBindVertexArray(m_emptyVAO); // Bind our Vertex Array Object so we can use it
	glBindVertexArray(0);
}

void JelloCube::reset()
{
	m_t = 0.0;
	generate();
}

void JelloCube::setK(double _k)
{
	m_k = _k;
}

void JelloCube::setDamping(double _damping)
{
	m_damping = _damping;
}

void JelloCube::setTimeStep(double _t)
{
	m_timestep = _t;
}

void JelloCube::generate()
{
	// std::cout<<"size of GLfloat: "<<sizeof(GLfloat)<<"\n";
	// std::cout<<"size of GLuint: "<<sizeof(GLuint)<<"\n";

	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeInitPass");

	ngl::Vec3 topRight = ngl::Vec3(1.0, 1.0, 1.0);
	ngl::Vec3 bottomLeft = ngl::Vec3(0.0, 0.0, 0.0);
	ngl::Vec3 span = topRight - bottomLeft;
	ngl::Vec3 step = span / ngl::Vec3(m_sizeX, m_sizeY, m_sizeZ);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	shader->setUniform("u_bottomLeft", bottomLeft);
	shader->setUniform("u_step", step);

	// create an atomic counter to get the number of required springs
	GLuint springCounter;
	genAtomicCounter(springCounter);
	// bind the atomic counter
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, springCounter);

	// create the texture to store the positions of the mass points
	unsigned int numMasses = m_sizeX * m_sizeY * m_sizeZ;
	std::vector<SSBO_State> massPointsArray(numMasses);

	// generate an ssbo for the masses
	genSSBO(m_massBufferId, numMasses * sizeof(SSBO_State), &massPointsArray[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);

	// set to only count the number of springs
	shader->setUniform("u_springWrite", false);
	// dispatch compute shader
	glDispatchCompute(m_sizeX, m_sizeY, m_sizeZ);
	// glMemoryBarrier(GL_ALL_BARRIER_BITS);

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

	std::vector<SSBO_Spring> springsArray(m_springCount);

	// generate an ssbo for the springs
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

void JelloCube::update()
{
	// std::cout<<"updating springs\n";
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeSpringPass");

	shader->setUniform("u_currentTime", m_t);
	shader->setUniform("u_timeStep", m_timestep);
	shader->setUniform("u_k", m_k);
	shader->setUniform("u_damping", m_damping);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_springBufferId);

	shader->setUniform("u_springCount", GLint(m_springCount));
	shader->setUniform("u_writeMode", false);

	// dispatch compute shader to integrate springs
	// glDispatchCompute(ceil(m_springCount / 128.0), 1, 1);
	glDispatchCompute(m_springCount / 128.0, 1, 1);


	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_springBufferId);

	// dispatch compute shader to update springs
	shader->setUniform("u_writeMode", true);
	// glDispatchCompute(1, 1, 1);
	// glDispatchCompute(ceil(m_springCount / 128.0), 1, 1);
	glDispatchCompute(m_springCount / 128.0, 1, 1);

	// update the timestep for the next time
	m_t += m_timestep;

/*	// for (size_t i = 0; i < m_massPoints.size(); ++i)
	// {
	// 	*(m_massPoints[i]) += ngl::Vec3(-0.001 * sin(m_t + i), 0.001 * sin(m_t + i), 0.001 * cos(m_t + i));
	// 	// *(m_massPoints[i]) += ngl::Vec3(0.0);
	// }


	float intensity = 0.01;
	*(m_massPoints[int(m_t + 20)%124]) += ngl::Vec3(intensity * sin(m_t + 49.0), intensity * sin(m_t), intensity * cos(m_t + 7.0));*/
}
