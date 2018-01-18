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

#include <math.h>

//----------------------------------------------------------------------------------------------------------------------

NGLScene::NGLScene(QWidget *_parent, JelloCube *_cube) : QOpenGLWidget(_parent), m_jelloCube(_cube)
{
	// set this widget to have the initial keyboard focus
	setFocusPolicy (Qt::StrongFocus);
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	this->resize(_parent->size());
	m_timer.start();

	m_drawCollider = true;
	m_drawMesh = true;
	m_drawMassPoints = false;
	m_drawStructuralSprings = false;
	m_drawShearSprings = false;
	m_drawBendSprings = false;

	m_gBufferFBOId = 0;
	m_FBOWSPositionId = 0;
	m_FBOWSNormalId = 0;
	m_FBODepthId = 0;
	m_FBOAlbedoId = 0;
	m_FBOMetalRoughId = 0;
	m_isFBODirty = true;

	m_sphereRadius = 1.0f;

	startSimTimer();
}

NGLScene::~NGLScene()
{
	std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
	// delete framebuffer textures
	glDeleteTextures(1, &m_FBOWSPositionId);
	glDeleteTextures(1, &m_FBOWSNormalId);
	glDeleteTextures(1, &m_FBODepthId);
	glDeleteTextures(1, &m_FBOAlbedoId);
	glDeleteTextures(1, &m_FBOMetalRoughId);
	// delete framebuffer object
	glDeleteFramebuffers(1, &m_gBufferFBOId);
}

void NGLScene::resizeGL( int _w, int _h )
{
	m_cam.setProjection( 45.0f, static_cast<float>( _w ) / _h, 0.001f, 100.0f );
	m_win.width  = static_cast<int>(_w * devicePixelRatio());
	m_win.height = static_cast<int>(_h * devicePixelRatio());
	m_isFBODirty = true;
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

	// create the plane shader program
	shader->loadShader("basicShader",
		"shaders/basic_vert.glsl",
		"shaders/basic_frag.glsl");
	shader->use("basicShader");

	// create the output shader program
	shader->loadShader("outputPass",
		"shaders/screen_space_vert.glsl",
		"shaders/output_frag.glsl");
	shader->use("outputPass");
	shader->setUniform("WSPositionTex", 0);
	shader->setUniform("WSNormalTex", 1);
	shader->setUniform("depthTex", 2);
	shader->setUniform("albedoTex", 3);
	shader->setUniform("metalRoughTex", 4);
	setupSSAO();
	shader->setUniform("noiseTex", 5);

	// Grey Background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);

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
	prim->createSphere("skyBox", 50.0f, 16);
	// ground plane geometry
	prim->createTrianglePlane("ground", 100, 100, 20, 20, ngl::Vec3(0.0, 1.0, 0.0));
	// collision sphere geometry
	prim->createSphere("sphereCollider", 1.0f, 16);
	// generate screen aligned quad
	prim->createTrianglePlane("ScreenAlignedQuad", 2, 2, 1, 1, ngl::Vec3(0,1,0));

	m_jelloCube->initializeShaders();
}

void NGLScene::paintGL()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

	float currentFrame = m_timer.elapsed()*0.001f;
	// std::cout<<"FPS: "<<1.0f / m_deltaTime<<'\n';
	m_framesPerSecond = 1.0f / m_deltaTime;
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	/// first we reset the movement values
	float xDirection=0.0;
	float yDirection=0.0;
	// now we loop for each of the pressed keys in the the set
	// and see which ones have been pressed. If they have been pressed
	// we set the movement value to be an incremental value
	constexpr float s_update = 1.0f;
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

	//----------------------------------------------------------------------------------------------------------------------
	/// GBUFFER PASS
	//----------------------------------------------------------------------------------------------------------------------

	// Check if the FBO needs to be recreated. This occurs after a resize.
	if (m_isFBODirty)
	{
		initFBO();
	}
	// bind the gBuffer FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	m_transform.reset();
	// m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));

	if (m_drawMesh)
	{
		// draw jello mesh
		shader->use("jelloGeoShader");
		loadMatricesToShader();
		m_jelloCube->drawMesh();
	}

	// draw springs
	shader->use("springShader");
	shader->setUniform("u_drawStructuralSprings", m_drawStructuralSprings);
	shader->setUniform("u_drawBendSprings", m_drawBendSprings);
	shader->setUniform("u_drawShearSprings", m_drawShearSprings);
	loadMatricesToShader();
	m_jelloCube->drawSprings();

	// draw masses
	if (m_drawMassPoints)
	{
		shader->use("testShader");
		loadMatricesToShader();
		m_jelloCube->drawMasses();
	}

	// draw ground
	shader->use("basicShader");
	shader->setUniform("diffuse", ngl::Vec3(1.0, 1.0, 1.0));
	loadMatricesToShader();
	prim->draw("ground");

	m_transform.setPosition(m_cam.getEye());
	loadMatricesToShader();
	prim->draw("skyBox");

	if (m_drawCollider)
	{
		m_transform.reset();
		m_transform.setPosition(m_spherePos);
		m_transform.setScale(ngl::Vec3(m_sphereRadius, m_sphereRadius, m_sphereRadius));
		loadMatricesToShader();
		shader->setUniform("diffuse", ngl::Vec3(0.9, 0.1, 0.1));
		prim->draw("sphereCollider");
	}

	//----------------------------------------------------------------------------------------------------------------------
	/// OUTPUT PASS START
	//----------------------------------------------------------------------------------------------------------------------

	m_transform.reset();
	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);
	// use the output pass shader
	shader->use("outputPass");
	// send the window size
	shader->setUniform("windowSize", ngl::Vec2(m_win.width, m_win.height));
	// MVP for screenspace effects
	ngl::Mat4 SSMVP = ngl::Mat4(1.0f);
	SSMVP.rotateX(90);
	shader->setUniform("SSMVP", SSMVP);
	// send matrices to shader
	loadMatricesToShader();
	// bind the textures to their texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSPositionId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSNormalId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_FBODepthId);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_FBOAlbedoId);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_FBOMetalRoughId);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	// draw screen quad
	prim->draw("ScreenAlignedQuad");
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
	// m_spherePos = ngl::Vec3(sin(m_timer.elapsed() * 0.001f), 1.0f, 0.0);
	m_spherePos = ngl::Vec3(1.5f + 10.0 * cos(m_timer.elapsed() * 0.001f), 1.0f, 1.5f);
	m_jelloCube->update(m_spherePos, m_sphereRadius);
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
