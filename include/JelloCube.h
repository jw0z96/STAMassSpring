#ifndef JELLOCUBE_H
#define JELLOCUBE_H

#include <QObject>
#include <ngl/Vec3.h>
#include <vector>
#include <memory>

#include <math.h>

#include "Spring.h"

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
	/// @brief get our mass points (for drawing)
	//----------------------------------------------------------------------------------------------------------------------
	// inline std::vector<std::shared_ptr<ngl::Vec3>> getMassPoints(){return m_massPoints;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief get our springs (for drawing)
	//----------------------------------------------------------------------------------------------------------------------
	// inline std::vector<Spring>* getStructuralSprings(){return &m_structuralSprings;};
	// inline std::vector<Spring>* getBendSprings(){return &m_bendSprings;};
	// inline std::vector<Spring>* getShearSprings(){return &m_shearSprings;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for generating 1D textures
	//----------------------------------------------------------------------------------------------------------------------
	void gen1DTexture(unsigned int &texId, unsigned int size, GLenum internalFormat, GLenum format, GLenum type, const GLvoid* data);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief helper function for generating an atomic counter
	//----------------------------------------------------------------------------------------------------------------------
	void genAtomicCounter(unsigned int &buffer);
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
	/// @brief draw the jello cube
	//----------------------------------------------------------------------------------------------------------------------
	void drawMasses();
	void drawSprings();

	private:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief build our arrays
	//----------------------------------------------------------------------------------------------------------------------
	void generate();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of points that make up our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	// std::vector<std::shared_ptr<ngl::Vec3>> m_massPoints;
	GLuint m_massPointsPositionTex;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief array of springs that connect our jello cube
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int m_springCount;
	GLuint m_springsRestingLengthTex;
	GLuint m_springsStatePositionTex;
	GLuint m_springsStateVelocityTex;
	GLuint m_springsStartIndexTex;
	GLuint m_springsEndIndexTex;
	// std::vector<Spring> m_structuralSprings;
	// std::vector<Spring> m_bendSprings;
	// std::vector<Spring> m_shearSprings;
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
