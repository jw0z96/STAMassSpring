#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include "WindowParams.h"
#include <ngl/Transformation.h>
#include "FirstPersonCamera.h"
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QSet>

#include <vector>
#include <memory>

#include "Spring.h"

//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt QOpenGLWidget and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWidget
{
	Q_OBJECT

	public:
	/// @brief Constructor for GLWindow
	/// @param [in] _parent the parent window to create the GL context in
	//----------------------------------------------------------------------------------------------------------------------
	NGLScene(QWidget *_parent );
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief dtor must close down ngl and release OpenGL resources
	//----------------------------------------------------------------------------------------------------------------------
	~NGLScene();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the initialize class is called once when the window is created and we have a valid GL context
	/// use this to setup any default GL stuff
	//----------------------------------------------------------------------------------------------------------------------
	void initializeGL() override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called everytime we want to draw the scene
	//----------------------------------------------------------------------------------------------------------------------
	void paintGL() override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called everytime we resize
	//----------------------------------------------------------------------------------------------------------------------
	void resizeGL(int _w, int _h) override;

	public slots:

	void toggleMassPoints(){m_drawMassPoints = !m_drawMassPoints; update();};
	void toggleStructuralSpring(){m_drawStructuralSprings = !m_drawStructuralSprings; update();};
	void toggleShearSpring(){m_drawShearSprings = !m_drawShearSprings; update();};
	void toggleBendSpring(){m_drawBendSprings = !m_drawBendSprings; update();};

	private:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the windows params such as mouse and rotations etc
	//----------------------------------------------------------------------------------------------------------------------
	WinParams m_win;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief used to store the global mouse transforms
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Mat4 m_mouseGlobalTX;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Our Camera
	//----------------------------------------------------------------------------------------------------------------------
	FirstPersonCamera m_cam;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief transformation stack for the gl transformations etc
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Transformation m_transform;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the model position for mouse movement
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Vec3 m_modelPos;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timing for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_deltaTime = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief frame time for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_lastFrame = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timer for measurment
	/// -----------------------------------------------------------------------------
	QElapsedTimer m_timer;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the keys being pressed
	//----------------------------------------------------------------------------------------------------------------------
	QSet<Qt::Key> m_keysPressed;
	//----------------------------------------------------------------------------------------------------------------------
	/* SIMULATION HELPER FUNCTIONS */
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief an array of points to represent our masses
	//----------------------------------------------------------------------------------------------------------------------
	std::vector<ngl::Vec3> m_massPts;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief an array of springs to represent our structural springs
	//----------------------------------------------------------------------------------------------------------------------
	std::vector<Spring> m_structuralSprings;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	void initScene();
	//----------------------------------------------------------------------------------------------------------------------
	/* RENDER HELPER FUNCTIONS */
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief bools to toggle the drawing of physics elements
	//----------------------------------------------------------------------------------------------------------------------
	bool m_drawMassPoints;
	bool m_drawStructuralSprings;
	bool m_drawShearSprings;
	bool m_drawBendSprings;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	void loadMatricesToShader();
	//----------------------------------------------------------------------------------------------------------------------
	/* EVENT HANDLING */
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Qt Event called when a key is pressed
	/// @param [in] _event the Qt event to query for size etc
	//----------------------------------------------------------------------------------------------------------------------
	void keyPressEvent(QKeyEvent *_event) override;
	void keyReleaseEvent(QKeyEvent *_event) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called every time a mouse is moved
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mouseMoveEvent (QMouseEvent * _event ) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse button is pressed
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mousePressEvent ( QMouseEvent *_event) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse button is released
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mouseReleaseEvent ( QMouseEvent *_event ) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse wheel is moved
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void wheelEvent( QWheelEvent *_event) override;
};

#endif
