#include "NGLScene.h"
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>

#include <random>

void NGLScene::setupSSAO()
{
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();

	//SOURCE FROM https://learnopengl.com/#!Advanced-Lighting/SSAO BEGINS HERE
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); //random floats between 0.0 - 1.0
	std::default_random_engine generator; //random number generator
	std::vector<ngl::Vec3> ssaoKernel; //array for the kernel

	//SETUP SSAO KERNEL
	int kernelSize = 64;
	for (GLuint i = 0; i < kernelSize; ++i)
	{
		ngl::Vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
			);
		sample.normalize();
		sample *= randomFloats(generator);
		float scale = float(i) / kernelSize;
		scale = ngl::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		shader->setUniform(("samples[" + std::to_string(i) + "]").c_str(), sample);
	}

	//SETUP SMALL NOISE TEXTURE
	std::vector<ngl::Vec3> ssaoNoise;
	int noiseSize = 4;
	for (int i = 0; i < noiseSize*noiseSize; i++)
	{
		ngl::Vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &m_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::initFBO()
{
	std::cout<<"initFBO call\n";

	// First delete the FBO if it has been created previously
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteTextures(1, &m_FBOWSPositionId);
		glDeleteTextures(1, &m_FBOWSNormalId);
		glDeleteTextures(1, &m_FBODepthId);
		glDeleteTextures(1, &m_FBOAlbedoId);
		glDeleteTextures(1, &m_FBOMetalRoughId);
		glDeleteFramebuffers(1, &m_gBufferFBOId);
	}

	auto setParams=[]()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	};

	// Create the frame buffer
	glGenFramebuffers(1, &m_gBufferFBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);

	// Generate a texture to write the Position to
	glGenTextures(1, &m_FBOWSPositionId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSPositionId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_win.width, m_win.height, 0, GL_RGB, GL_FLOAT, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOWSPositionId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Normals to
	glGenTextures(1, &m_FBOWSNormalId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSNormalId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_win.width, m_win.height, 0, GL_RGB, GL_FLOAT, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_FBOWSNormalId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// The depth buffer is rendered to a texture buffer too,
	glGenTextures(1, &m_FBODepthId);
	glBindTexture(GL_TEXTURE_2D, m_FBODepthId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_win.width, m_win.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_FBODepthId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Albedo to
	glGenTextures(1, &m_FBOAlbedoId);
	glBindTexture(GL_TEXTURE_2D, m_FBOAlbedoId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_win.width, m_win.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_FBOAlbedoId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Metallness and Roughness to
	glGenTextures(1, &m_FBOMetalRoughId);
	glBindTexture(GL_TEXTURE_2D, m_FBOMetalRoughId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_win.width, m_win.height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_FBOMetalRoughId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Set the fragment shader output targets DEPTH_ATTACHMENT is done automatically apparently
	unsigned int drawBufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, drawBufs);

	// Check it is ready to rock and roll
	checkFrameBuffer();
	// Unbind the framebuffer to revert to default render pipeline
	glBindFramebuffer(GL_FRAMEBUFFER, 1);

	m_isFBODirty = false;
}

//----------------------------------------------------------------------------------------------------------------------
/// The following section is modified from:
/// Richard Southern (2017): Realtime Rendering Assignment Materials
/// Available from https://mybu.bournemouth.ac.uk/webapps/blackboard/content/listContent.jsp?course_id=_50978_1&content_id=_1375211_1
void NGLScene::checkFrameBuffer() const
{
	switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		case GL_FRAMEBUFFER_UNDEFINED:
		std::cerr<<"GL_FRAMEBUFFER_UNDEFINED: returned if target is the default framebuffer, but the default framebuffer does not exist.\n";
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: returned if any of the framebuffer attachment points are framebuffer incomplete.\n";
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: returned if the framebuffer does not have at least one image attached to it.\n";
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAWBUFFERi.\n";
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.\n";
		break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr<<"GL_FRAMEBUFFER_UNSUPPORTED: returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions. GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.\n";
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.\n";
		break;
		case GL_FRAMEBUFFER_COMPLETE:
		std::cerr<<"GL_FRAMEBUFFER_COMPLETE: returned if everything is groovy!\n";
		break;
		default:
		std::cerr<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<": Undefined framebuffer return value: possible error elsewhere?\n";
		break;
	}
}
/// end of citation

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::loadMatricesToShader()
{
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::Mat4 MV;
	ngl::Mat4 MVP;
	ngl::Mat3 normalMatrix;
	ngl::Mat4 M;
	M            = m_transform.getMatrix() * m_mouseGlobalTX ;
	MV           = m_cam.getView() * M;
	MVP          = m_cam.getVP() * M;

	normalMatrix = MV;
	normalMatrix.inverse().transpose();
	shader->setUniform("MVP", MVP);
	shader->setUniform("normalMatrix", normalMatrix);
	shader->setUniform("M", M);
	shader->setUniform("MV", MV);
	shader->setUniform("camPos",m_cam.getEye());
}
