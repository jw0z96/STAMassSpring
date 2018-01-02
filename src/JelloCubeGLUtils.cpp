#include "JelloCube.h"

#include <iostream>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

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
	glBindTexture(GL_TEXTURE_1D, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void JelloCube::draw()
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, m_massPointsPositionTex);

	for (int i = 0; i < (m_sizeX * m_sizeY * m_sizeZ); ++i)
	{
		shader->setUniform("u_index", GLint(i));
		prim->draw("cube");
	}
	// std::cout<<"hello!!!!!!\n";
}