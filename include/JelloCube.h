#ifndef JELLOCUBE_H
#define JELLOCUBE_H

#include <QObject>
#include <ngl/Vec4.h>
#include <vector>
#include <memory>

#include <math.h>

struct SSBO_Mass
{
	ngl::Vec4 m_position;
	ngl::Vec4 m_velocity;
	ngl::Vec4 m_forces;
};

struct SSBO_Spring
{
	GLuint m_start; float pad0[3];
	GLuint m_end; float pad1[3];
	GLfloat m_restingLength; float pad2[3];
	ngl::Vec4 m_velocity;
	GLuint m_springType; float pad3[3];
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
	void calculateSpringForces();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief update testing function
	//----------------------------------------------------------------------------------------------------------------------
	void calculateExternalForces();
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
	/// @brief set the mass of our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	void setMass(double _mass);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the gravity constant
	//----------------------------------------------------------------------------------------------------------------------
	void setGravity(double _gravity);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the simulation timestep
	//----------------------------------------------------------------------------------------------------------------------
	void setTimeStep(double _t);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief set the simulation timestep
	//----------------------------------------------------------------------------------------------------------------------
	void setSubSteps(int _s);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief draw the jello cube or masses individually
	//----------------------------------------------------------------------------------------------------------------------
	void drawMesh();
	void drawMasses();
	void drawSprings();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief get some counters for the ui
	//----------------------------------------------------------------------------------------------------------------------
	inline unsigned int getMassCount(){return m_massCount;};
	inline unsigned int getSpringCount(){return m_springCount;};

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
	/// @brief the number of masses currently in our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_massCount;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of masses that make up our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_massBufferId;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of springs that connect our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_springBufferId;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the spring constant to be used for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	float m_k;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the damping constant to be used for all of the springs
	//----------------------------------------------------------------------------------------------------------------------
	float m_damping;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the mass of our jello cube (entire mass, not per mass point)
	//----------------------------------------------------------------------------------------------------------------------
	float m_mass;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief gravity
	//----------------------------------------------------------------------------------------------------------------------
	float m_gravity;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the current time
	//----------------------------------------------------------------------------------------------------------------------
	float m_t;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the time step used for update default 0.1
	//----------------------------------------------------------------------------------------------------------------------
	float m_timestep;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the number of substeps to be calculated
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_subSteps;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the dimensions of our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_sizeX, m_sizeY, m_sizeZ;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief number of quads needed to make jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_numQuads;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief empty VAO for procedural geometry
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_emptyVAO;
};

#endif // JELLOCUBE_H
