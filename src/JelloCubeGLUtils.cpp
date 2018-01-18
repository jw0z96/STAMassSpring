#include "JelloCube.h"

#include <iostream>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::genAtomicCounter(unsigned int &buffer)
{
	if (buffer)
		glDeleteBuffers(1, &buffer);

	// initial value
	GLuint initVal = 0;
	// declare and generate a buffer object name
	glGenBuffers(1, &buffer);
	// bind the buffer and define its initial storage capacity
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initVal, GL_DYNAMIC_DRAW);
	// unbind the buffer
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::genSSBO(unsigned int &buffer, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
	if (buffer)
		glDeleteBuffers(1, &buffer);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::drawMesh()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	// glEnable(GL_CULL_FACE);

	glBindVertexArray(m_emptyVAO);
	// draw half the number of quads (can mirror the other side)
	glDrawArrays(GL_POINTS, 0, (m_numQuads / 2.0));
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::drawMasses()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	for (int i = 0; i < (m_sizeX * m_sizeY * m_sizeZ); ++i)
	{
		shader->setUniform("u_index", GLint(i));
		prim->draw("cube");
	}
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::drawSprings()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_massBufferId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_springBufferId);

	glBindVertexArray(m_emptyVAO);

	// glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.5);
	glDrawArrays(GL_LINES, 0, m_springCount * 2);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_1D, 0);
}
