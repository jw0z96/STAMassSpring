#ifndef JELLOCUBE_H
#define JELLOCUBE_H

#include <QObject>
#include <ngl/Vec4.h>
#include <vector>
#include <memory>

#include <math.h>

// #include "Spring.h"

struct SSBO_State
{
	ngl::Vec4 m_position;
	ngl::Vec4 m_velocity;
};

struct SSBO_Spring
{
	SSBO_State m_state;
	GLuint m_start; float pad0[3];
	GLuint m_end; float pad1[3];
	GLfloat m_restingLength; float pad2[3];
};

class JelloCube : public QObject
{
	Q_OBJECT

public:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Constructor for GLWindow
	//----------------------------------------------------------------------------------------------------------------------
	JelloCube(double _k = 5.0, double _damping = 2.0);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Destructor for GLWindow
	//----------------------------------------------------------------------------------------------------------------------
	~JelloCube();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief update testing function
	//----------------------------------------------------------------------------------------------------------------------
	void update();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief update testing function
	//----------------------------------------------------------------------------------------------------------------------
	void calculateSpringForces(unsigned int _subSteps);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief update testing function
	//----------------------------------------------------------------------------------------------------------------------
	void calculateExternalForces(unsigned int _subSteps);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for generating 1D textures
	//----------------------------------------------------------------------------------------------------------------------
	void gen1DTexture(unsigned int &texId, unsigned int size, GLenum internalFormat, GLenum format, GLenum type, const GLvoid* data);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for generating an atomic counter
	//----------------------------------------------------------------------------------------------------------------------
	void genAtomicCounter(unsigned int &buffer);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for generating an SSBO
	//----------------------------------------------------------------------------------------------------------------------
	void genSSBO(unsigned int &buffer, GLsizeiptr size, const GLvoid * data, GLenum usage);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for initializing the textures and compute shaders
	//----------------------------------------------------------------------------------------------------------------------
	void initializeShaders();

	public slots:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief reset the state
	//----------------------------------------------------------------------------------------------------------------------
	void reset();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the spring constant for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	void setK(double _k);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the damping constant for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	void setDamping(double _damping);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the simulation timestep
	//----------------------------------------------------------------------------------------------------------------------
	void setTimeStep(double _t);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief draw the jello cube or masses individually
	//----------------------------------------------------------------------------------------------------------------------
	void drawMasses();
	void drawSprings();

private:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief build our arrays
	//----------------------------------------------------------------------------------------------------------------------
	void generate();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the number of springs currently in our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_springCount;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of masses that make up our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_massBufferId;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of springs that connect our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_springBufferId;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the time step used for update default 0.1
	//----------------------------------------------------------------------------------------------------------------------
	float m_timestep;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the current time
	//----------------------------------------------------------------------------------------------------------------------
	float m_t;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the spring constant to be used for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	float m_k;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the damping constant to be used for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	float m_damping;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the dimensions of our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_sizeX, m_sizeY, m_sizeZ;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief empty VAO for procedural geometry
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_emptyVAO;
};

#endif // JELLOCUBE_H
