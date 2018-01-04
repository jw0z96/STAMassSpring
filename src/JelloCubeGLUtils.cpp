#include "JelloCube.h"

#include <iostream>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

void _check_gl_error(int _line)
{
	GLenum err (glGetError());

	while(err!=GL_NO_ERROR)
	{
		std::string error;

		switch(err)
		{
			case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		std::cerr<<"GL_"<<error.c_str()<<" line: "<<_line<<std::endl;
		err=glGetError();
	}
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::gen1DTexture(unsigned int &texId, unsigned int size, GLenum internalFormat, GLenum format, GLenum type, const GLvoid* data)
{
	if (texId)
		glDeleteTextures(1, &texId);

	glGenTextures(1, &texId );
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, size, 0, format, type, data);
	_check_gl_error(42);
	glBindTexture(GL_TEXTURE_1D, 0);
}

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

void JelloCube::drawMasses()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, m_massPointsPositionTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, m_springsStartIndexTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, m_springsEndIndexTex);

	shader->setUniform("u_sizeX", GLint(m_sizeX));
	shader->setUniform("u_sizeY", GLint(m_sizeY));
	shader->setUniform("u_sizeZ", GLint(m_sizeZ));

	for (int i = 0; i < (m_sizeX * m_sizeY * m_sizeZ); ++i)
	{
		shader->setUniform("u_index", GLint(i));
		prim->draw("cube");
	}
	// std::cout<<"hello!!!!!!\n";
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::drawSprings()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glBindTexture(GL_TEXTURE_1D, m_springsStartIndexTex);
	glBindImageTexture(0, m_springsStartIndexTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16UI);
	glBindTexture(GL_TEXTURE_1D, m_springsEndIndexTex);
	glBindImageTexture(1, m_springsEndIndexTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16UI);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, m_massPointsPositionTex);

	glBindVertexArray(m_emptyVAO);

	// glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.5);
	glDrawArrays(GL_LINES, 0, m_springCount * 2);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_1D, 0);
}
