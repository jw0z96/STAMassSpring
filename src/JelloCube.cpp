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
	m_sizeX = 5;
	m_sizeY = 5;
	m_sizeZ = 5;
	// generate();
	// initializeShaders();
}

JelloCube::~JelloCube()
{
	std::cout<<"bye!\n";
	glDeleteTextures(1, &m_massPointsPositionTex);
	glDeleteTextures(1, &m_springsRestingLengthTex);
	glDeleteTextures(1, &m_springsStatePositionTex);
	glDeleteTextures(1, &m_springsStateVelocityTex);
	glDeleteTextures(1, &m_springsStartIndexTex);
	glDeleteTextures(1, &m_springsEndIndexTex);
	glDeleteVertexArrays(1, &m_emptyVAO);
}

void JelloCube::initializeShaders()
{
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();

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

void JelloCube::generate()
{
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
	gen1DTexture(m_massPointsPositionTex, numMasses, GL_RGBA32F, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_massPointsPositionTex);
	glBindImageTexture(0, m_massPointsPositionTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
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

	// bind the atomic counter
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, springCounter);
	// generate textures to store spring info
	// spring resting length
	gen1DTexture(m_springsRestingLengthTex, m_springCount, GL_R16F, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_springsRestingLengthTex);
	glBindImageTexture(1, m_springsRestingLengthTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);
	// spring state position
	gen1DTexture(m_springsStatePositionTex, m_springCount, GL_RGBA32F, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_springsStatePositionTex);
	glBindImageTexture(2, m_springsStatePositionTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	// spring state velocity
	gen1DTexture(m_springsStateVelocityTex, m_springCount, GL_RGBA32F, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_springsStateVelocityTex);
	glBindImageTexture(3, m_springsStateVelocityTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	// spring start and end indices
	gen1DTexture(m_springsStartIndexTex, m_springCount, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_springsStartIndexTex);
	glBindImageTexture(4, m_springsStartIndexTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16UI);
	gen1DTexture(m_springsEndIndexTex, m_springCount, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_1D, m_springsEndIndexTex);
	glBindImageTexture(5, m_springsEndIndexTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16UI);

	// set to write the contents of springs
	shader->setUniform("u_springWrite", true);

	// dispatch compute shader (this time to store springs)
	glDispatchCompute(m_sizeX, m_sizeY, m_sizeZ);

	// unbind textures
	glBindTexture(GL_TEXTURE_1D, 0);

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

	/*
	// populate the arrays of springs
	for (int i = 0; i < sx; ++i)
	{
		for (int j = 0; j < sy; ++j)
		{
			for (int k = 0; k < sz; ++k)
			{
				// the index at (i, j, k)
				int currentIndex = getIndex(i, j, k, sx, sy);

				// create structural springs
				if ((i + 1) < sx) // connect (i,j,k) to (i+1, j, k)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k, sx, sy)], m_k, m_damping);
				if ((j +1) < sy) // connect (i,j,k) to (i, j+1, k)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k, sx, sy)], m_k, m_damping);
				if ((k +1) < sz) // connect (i,j,k) to (i, j, k+1)
					m_structuralSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 1, sx, sy)], m_k, m_damping);

				// create bend springs
				if ((i + 2) < sx) // connect (i,j,k) to (i+2, j, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 2, j, k, sx, sy)], m_k, m_damping);
				if ((j +2) < sy) // connect (i,j,k) to (i, j+2, k)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 2, k, sx, sy)], m_k, m_damping);
				if ((k +2) < sz) // connect (i,j,k) to (i, j, k+2)
					m_bendSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j, k + 2, sx, sy)], m_k, m_damping);

				// create shear springs
				if ((i + 1) < sx && (j + 1) < sy) // connect (i,j,k) to (i+1,j+1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j + 1, k, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && (k + 1) < sz) // connect (i,j,k) to (i+1,j,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k + 1, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && k > 0) // connect (i,j,k) to (i+1,j,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j, k - 1, sx, sy)], m_k, m_damping);

				if ((i + 1) < sx && j > 0) // connect (i,j,k) to (i+1,j-1,k)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i + 1, j - 1, k, sx, sy)], m_k, m_damping);

				if ((j + 1) < sy && (k + 1) < sz) // connect (i,j,k) to (i,j+1,k+1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k + 1, sx, sy)], m_k, m_damping);

				if ((j + 1) < sy && k > 0) // connect (i,j,k) to (i,j+1,k-1)
					m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j + 1, k - 1, sx, sy)], m_k, m_damping);

				// if (j > 0 && (k + 1) < sz) // connect (i,j,k) to (i,j-1,k+1)
					// m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k + 1, sx, sy)]);

				// if (j > 0 && k > 0) // connect (i,j,k) to (i,j-1,k-1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i, j - 1, k - 1, sx, sy)]);

				// if (i > 0 && (j + 1) < sy) // connect (i,j,k) to (i-1,j+1,k)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j + 1, k, sx, sy)]);

				// if (i > 0 && (k + 1) < sz) // connect (i,j,k) to (i-1,j,k+1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k + 1, sx, sy)]);

				// if (i > 0 && k > 0) // connect (i,j,k) to (i-1,j,k-1)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j, k - 1, sx, sy)]);

				// if (i > 0 && j > 0) // connect (i,j,k) to (i-1,j-1,k)
				// 	m_shearSprings.emplace_back(m_massPoints[currentIndex], m_massPoints[getIndex(i - 1, j - 1, k, sx, sy)]);
			}
		}
	}*/
}

void JelloCube::update()
{
	// std::cout<<"updating springs\n";
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("jelloCubeSpringPass");

	shader->setUniform("u_currentTime", m_t);
	shader->setUniform("u_timeStep", m_timestep);

	// mass point positions
	glBindTexture(GL_TEXTURE_1D, m_massPointsPositionTex);
	glBindImageTexture(0, m_massPointsPositionTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	// spring resting length
	glBindTexture(GL_TEXTURE_1D, m_springsRestingLengthTex);
	glBindImageTexture(1, m_springsRestingLengthTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);
	// spring state position
	glBindTexture(GL_TEXTURE_1D, m_springsStatePositionTex);
	glBindImageTexture(2, m_springsStatePositionTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	// spring state velocity
	glBindTexture(GL_TEXTURE_1D, m_springsStateVelocityTex);
	glBindImageTexture(3, m_springsStateVelocityTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	// spring start and end indices
	glBindTexture(GL_TEXTURE_1D, m_springsStartIndexTex);
	glBindImageTexture(4, m_springsStartIndexTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16UI);
	glBindTexture(GL_TEXTURE_1D, m_springsEndIndexTex);
	glBindImageTexture(5, m_springsEndIndexTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16UI);

	shader->setUniform("u_springCount", GLint(m_springCount));
	shader->setUniform("u_writeMode", false);

	// dispatch compute shader to integrate springs
	glDispatchCompute(m_springCount, 1, 1);

	// dispatch compute shader to update springs
	shader->setUniform("u_writeMode", true);
	glDispatchCompute(1, 1, 1);

	// update the timestep for the next time
	m_t += m_timestep;

	glBindTexture(GL_TEXTURE_1D, 0);

/*	// for (size_t i = 0; i < m_massPoints.size(); ++i)
	// {
	// 	*(m_massPoints[i]) += ngl::Vec3(-0.001 * sin(m_t + i), 0.001 * sin(m_t + i), 0.001 * cos(m_t + i));
	// 	// *(m_massPoints[i]) += ngl::Vec3(0.0);
	// }


	float intensity = 0.01;
	*(m_massPoints[int(m_t + 20)%124]) += ngl::Vec3(intensity * sin(m_t + 49.0), intensity * sin(m_t), intensity * cos(m_t + 7.0));*/
}
