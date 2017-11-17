#include <QMouseEvent>
#include <QApplication>

#include "NGLScene.h"

#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>

//----------------------------------------------------------------------------------------------------------------------

NGLScene::NGLScene(QWidget *_parent, JelloCube *_cube) : QOpenGLWidget(_parent), m_jelloCube(_cube)
{
	// set this widget to have the initial keyboard focus
	setFocusPolicy (Qt::StrongFocus);
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	this->resize(_parent->size());
	m_timer.start();

	m_drawMassPoints = true;
	m_drawStructuralSprings = false;
	m_drawShearSprings = false;
	m_drawBendSprings = false;

	startSimTimer();
}

NGLScene::~NGLScene()
{
	std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL( int _w, int _h )
{
	m_cam.setProjection( 45.0f, static_cast<float>( _w ) / _h, 0.001f, 100.0f );
	m_win.width  = static_cast<int>(_w * devicePixelRatio());
	m_win.height = static_cast<int>(_h * devicePixelRatio());
}

void NGLScene::initializeGL()
{
	// we must call this first before any other GL commands to load and link the
	// gl commands from the lib, if this is not done program will crash
	ngl::NGLInit::instance();

	ngl::ShaderLib *shader=ngl::ShaderLib::instance();

	// create the gBuffer shader program
	shader->loadShader("testShader",
		"shaders/test_vert.glsl",
		"shaders/test_frag.glsl");
	shader->use("testShader");

	// Grey Background
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
	// glDisable(GL_BLEND);

	ngl::Vec3 from(0.0, 1.0, 4.0);
	ngl::Vec3 to(0.0, 0.0, 0.0);
	ngl::Vec3 up(0.0, 1.0, 0.0);
	m_cam.set(from, to, up);
	// set the shape using FOV 45 Aspect Ratio based on Width and Height
	// The final two are near and far clipping planes of 0.5 and 10
	m_cam.setProjection(50,(float)1024/720,1.0f,800.0f);

	// as re-size is not explicitly called we need to do this.
	glViewport(0,0,width(),height());

	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	prim->createSphere("sphere",0.1f,1);
}

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
	shader->setUniform("camPos",m_cam.getEye());
}

void NGLScene::paintGL()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	shader->use("testShader");

	float currentFrame = m_timer.elapsed()*0.001f;
	// std::cout<<"Current Frame "<<currentFrame<<'\n';
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
	/// first we reset the movement values
	float xDirection=0.0;
	float yDirection=0.0;
	// now we loop for each of the pressed keys in the the set
	// and see which ones have been pressed. If they have been pressed
	// we set the movement value to be an incremental value
	constexpr float s_update = 0.1f;
	foreach(Qt::Key key, m_keysPressed)
	{
		switch (key)
		{
			case Qt::Key_Left :  { yDirection=-s_update; break;}
			case Qt::Key_Right : { yDirection=s_update; break;}
			case Qt::Key_Up :		 { xDirection=s_update; break;}
			case Qt::Key_Down :  { xDirection=-s_update; break;}
			default : break;
		}
	}
	// if the set is non zero size we can update the ship movement
	// then tell openGL to re-draw
	if (m_keysPressed.size() !=0)
	{
		m_cam.move(xDirection,yDirection,m_deltaTime);
	}

	m_transform.reset();

	auto massPoints = m_jelloCube->getMassPoints();

	if (m_drawMassPoints)
	{
		for(auto point : massPoints)
		{
			m_transform.setPosition(*point);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			loadMatricesToShader();
			prim->draw("cube");
		}
	}

	(*shader)[ngl::nglColourShader]->use();

	if (m_drawStructuralSprings)
	{
		std::vector<Spring> & springsRef = *(m_jelloCube->getStructuralSprings());
		for (size_t i = 0; i < springsRef.size(); ++i)
		{
			ngl::Vec3 start = *(springsRef[i].getStartPoint());
			ngl::Vec3 diff = (*(springsRef[i].getEndPoint()) - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
			//set colour to highlight stress
			float stretchFactor = diff.length() - springsRef[i].getRestingLength();
			shader->setUniform("Colour", 10.0 * abs(stretchFactor), 0.0f, 0.0f, 1.0f);
			for (int j = buffer; j < (count - buffer); ++j)
			{
				m_transform.setPosition(start + ((float(j)/float(count)) * diff));
				loadMatricesToShader();
				prim->draw("sphere");
			}
		}
	}

	if (m_drawBendSprings)
	{
		std::vector<Spring> & springsRef = *(m_jelloCube->getBendSprings());
		for (size_t i = 0; i < springsRef.size(); ++i)
		{
			ngl::Vec3 start = *(springsRef[i].getStartPoint());
			ngl::Vec3 diff = (*(springsRef[i].getEndPoint()) - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
			//set colour to highlight stress
			float stretchFactor = diff.length() - springsRef[i].getRestingLength();
			shader->setUniform("Colour", 10.0 * abs(stretchFactor), 0.0f, 0.0f, 1.0f);
			for (int j = buffer; j < (count - buffer); ++j)
			{
				m_transform.setPosition(start + ((float(j)/float(count)) * diff));
				loadMatricesToShader();
				prim->draw("sphere");
			}
		}
	}

	if (m_drawShearSprings)
	{
		std::vector<Spring> & springsRef = *(m_jelloCube->getShearSprings());
		for (size_t i = 0; i < springsRef.size(); ++i)
		{
			ngl::Vec3 start = *(springsRef[i].getStartPoint());
			ngl::Vec3 diff = (*(springsRef[i].getEndPoint()) - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
			//set colour to highlight stress
			float stretchFactor = diff.length() - springsRef[i].getRestingLength();
			shader->setUniform("Colour", 10.0 * abs(stretchFactor), 0.0f, 0.0f, 1.0f);
			for (int j = buffer; j < (count - buffer); ++j)
			{
				m_transform.setPosition(start + ((float(j)/float(count)) * diff));
				loadMatricesToShader();
				prim->draw("sphere");
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
	// add to our keypress set the values of any keys pressed
	m_keysPressed += static_cast<Qt::Key>(_event->key());

	// this method is called every time the main window recives a key event.
	// we then switch on the key value and set the camera in the GLWindow
	switch (_event->key())
	{
	// escape key to quite
		case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
	// turn on wirframe rendering
		case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
	// turn off wire frame
		case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
	// show full screen
		case Qt::Key_F : showFullScreen(); break;
	// show windowed
		case Qt::Key_N : showNormal(); break;
		default : break;
	}
	// finally update the GLWindow and re-draw
	//if (isExposed())
	update();
}

void NGLScene::keyReleaseEvent( QKeyEvent *_event	)
{
	// remove from our key set any keys that have been released
	m_keysPressed -= static_cast<Qt::Key>(_event->key());
}

void NGLScene::timerEvent( QTimerEvent *)
{
	m_jelloCube->update();
	update();
}

void NGLScene::startSimTimer()
{
	startTimer(10); // 10ms
	// m_timerStart = startTimer(m_timerValue);
}

void NGLScene::stopSimTimer()
{
	// killTimer(m_timerStart);
}
