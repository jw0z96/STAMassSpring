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
	// initialize the scene
	// initScene();

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

// void NGLScene::initScene()
// {
	// int sx = 5;
	// int sy = 5;
	// int sz = 5;

	// ngl::Vec3 topRight = ngl::Vec3(1.0, 1.0, 1.0);
	// ngl::Vec3 bottomLeft = ngl::Vec3(0.0, 0.0, 0.0);

	// ngl::Vec3 span = topRight - bottomLeft;
	// ngl::Vec3 step = span / ngl::Vec3(sx, sy, sz);

	// // populate the array of masses
	// for (int i = 0; i < sx; ++i)
	// {
	// 	for (int j = 0; j < sy; ++j)
	// 	{
	// 		for (int k = 0; k < sz; ++k)
	// 		{
	// 			m_massPts.push_back(ngl::Vec3(i, j, k) * step + bottomLeft);
	// 		}
	// 	}
	// }

	// // populate the arrays of springs
	// for (int i = 0; i < sx; ++i)
	// {
	// 	for (int j = 0; j < sy; ++j)
	// 	{
	// 		for (int k = 0; k < sz; ++k)
	// 		{
	// 			// get the array index of the starting point
	// 			int startIndex = (k * sx * sy) + (j * sx) + i;

	// 			// create structural springs
	// 			if ((i + 1) < sx) // connect (i,j,k) to (i+1, j, k)
	// 				m_structuralSprings.push_back(Spring(startIndex,(k * sx * sy) + (j * sx) + (i + 1)));
	// 			if ((j +1) < sy) // connect (i,j,k) to (i, j+1, k)
	// 				m_structuralSprings.push_back(Spring(startIndex,(k * sx * sy) + ((j + 1) * sx) + i));
	// 			if ((k +1) < sz) // connect (i,j,k) to (i, j, k+1)
	// 				m_structuralSprings.push_back(Spring(startIndex,((k + 1) * sx * sy) + (j * sx) + i));

	// 			// create bend springs
	// 			if ((i + 2) < sx) // connect (i,j,k) to (i+2, j, k)
	// 				m_bendSprings.push_back(Spring(startIndex,(k * sx * sy) + (j * sx) + (i + 2)));
	// 			if ((j +2) < sy) // connect (i,j,k) to (i, j+2, k)
	// 				m_bendSprings.push_back(Spring(startIndex,(k * sx * sy) + ((j + 2) * sx) + i));
	// 			if ((k +2) < sz) // connect (i,j,k) to (i, j, k+2)
	// 				m_bendSprings.push_back(Spring(startIndex,((k + 2) * sx * sy) + (j * sx) + i));

	// 			// create shear springs
	// 			if ((i + 1) < sx && (j + 1) < sy)
	// 				m_shearSprings.push_back(Spring(startIndex,((k) * sx * sy) + ((j + 1) * sx) + (i + 1))); // connect (i,j,k) to (i+1,j+1,k)

	// 			if ((i + 1) < sx && (k + 1) < sz)
	// 				m_shearSprings.push_back(Spring(startIndex,((k + 1) * sx * sy) + ((j) * sx) + (i + 1))); // connect (i,j,k) to (i+1,j,k+1)

	// 			if ((i + 1) < sx && k > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,((k - 1) * sx * sy) + ((j) * sx) + (i + 1))); // connect (i,j,k) to (i+1,j,k-1)

	// 			if ((i + 1) < sx && j > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,(k * sx * sy) + ((j - 1) * sx) + (i + 1))); // connect (i,j,k) to (i+1,j-1,k)

	// 			if ((j + 1) < sy && (k + 1) < sz)
	// 				m_shearSprings.push_back(Spring(startIndex,((k + 1) * sx * sy) + ((j + 1) * sx) + (i))); // connect (i,j,k) to (i,j+1,k+1)

	// 			if ((j + 1) < sy && k > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,((k - 1) * sx * sy) + ((j + 1) * sx) + (i))); // connect (i,j,k) to (i,j+1,k-1)

	// 			if (j > 0 && (k + 1) < sz)
	// 				m_shearSprings.push_back(Spring(startIndex,((k + 1) * sx * sy) + ((j - 1) * sx) + (i))); // connect (i,j,k) to (i,j-1,k+1)

	// 			if (j > 0 && k > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,((k - 1) * sx * sy) + ((j - 1) * sx) + (i))); // connect (i,j,k) to (i,j-1,k-1)

	// 			if (i > 0 && (j + 1) < sy)
	// 				m_shearSprings.push_back(Spring(startIndex,(k * sx * sy) + ((j + 1) * sx) + (i - 1))); // connect (i,j,k) to (i-1,j+1,k)

	// 			if (i > 0 && (k + 1) < sz)
	// 				m_shearSprings.push_back(Spring(startIndex,((k + 1) * sx * sy) + ((j) * sx) + (i - 1))); // connect (i,j,k) to (i-1,j,k+1)

	// 			if (i > 0 && k > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,((k - 1) * sx * sy) + ((j) * sx) + (i - 1))); // connect (i,j,k) to (i-1,j,k-1)

	// 			if (i > 0 && j > 0)
	// 				m_shearSprings.push_back(Spring(startIndex,(k * sx * sy) + ((j - 1) * sx) + (i - 1))); // connect (i,j,k) to (i-1,j-1,k)
	// 		}
	// 	}
	// }
// }

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

	ngl::Vec3 from(1.0, 1.0, 1.0);
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
	shader->setUniform( "MVP", MVP );
	shader->setUniform( "normalMatrix", normalMatrix );
	shader->setUniform( "M", M );
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

	m_jelloCube->update();

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

	if (m_drawStructuralSprings)
	{
		std::vector<Spring> & springsRef = *(m_jelloCube->getStructuralSprings());
		for (size_t i = 0; i < springsRef.size(); ++i)
		{
			ngl::Vec3 start = *(springsRef[i].getStartPoint());
			ngl::Vec3 end = *(springsRef[i].getEndPoint());
			ngl::Vec3 diff = (end - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
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
			ngl::Vec3 end = *(springsRef[i].getEndPoint());
			ngl::Vec3 diff = (end - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
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
			ngl::Vec3 end = *(springsRef[i].getEndPoint());
			ngl::Vec3 diff = (end - start);
			m_transform.setScale(ngl::Vec3(0.1, 0.1, 0.1));
			int count = 20;
			int buffer = count / 5;
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

// void NGLScene::simulateJello()
// {
	/*// Specify stiffness, time step, and mass
	float ks = 100000.0;
	double dt = 0.0001;
	float m = 1.0;

	// // # 3) Specify the indices of boundary nodes
	// // # bouIds = [0, sx, sy*(sx+1), sy*(sx+1)+sx] # (0,0,0), (sx, 0, 0), (0, sy, 0), (sx, sy, 0)
	// bouIds = []
	// for i in range(sx+1):
	// 	for j in range(sy+1):
	// 		bouIds.append(j*(sx+1)+i)

	// // # 4) Specify external forces
	// forces = []
	// for i in range(sx+1):
	// 	forces.append([sz*(sx+1)*(sy+1)+i, 0, 100000, 0]) # (i,0,sz)

	// rest direct spring lens.
	std::vector<ngl::Vec3> structuralSpringsRestingLength;
	// structuralSpringsRestingLength.reserve(m_structuralSprings.size());
	for (size_t i = 0; i < m_structuralSprings.size(); ++i)
	{
		auto spring = m_structuralSprings[i];
		ngl::Vec3 start = m_massPts[spring.startPoint];
		ngl::Vec3 end = m_massPts[spring.endPoint];
		structuralSpringsRestingLength.push_back(ngl::Vec3(end - start));
	}

	std::vector<ngl::Vec3> massPtsCurr = m_massPts;
	std::vector<ngl::Vec3> velCurr;
	velCurr.reserve(m_massPts.size());
	std::fill(velCurr.begin(), velCurr.end(), ngl::Vec3(0.0, 0.0, 0.0));

	int maxIter = 300;

	for (int i = 0; i < maxIter; ++i)
	{
	// 	std::vector<ngl::Vec3> force_per_vert;
	// 	force_per_vert.reserve(massPtsCurr.size());
	// 	ngl::Vec3 force = ngl::Vec3(100.0, 0.0, 0.0);
	// 	// std::fill(force_per_vert.begin(), force_per_vert.end(), force);
	// 	force_per_vert[20] = force;
	// 	force_per_vert[30] = force;
	// 	force_per_vert[40] = force;
	// 	force_per_vert[50] = force;
	// 	force_per_vert[60] = force;

	// 	for (int j = 0; j < m_structuralSprings.size(); ++j)
	// 	{
	// 		int startPoint = m_structuralSprings[j].startPoint;
	// 		int endPoint = m_structuralSprings[j].endPoint;
	// 		ngl::Vec3 axis = massPtsCurr[endPoint] - massPtsCurr[startPoint];
	// 		ngl::Vec3 newLen = axis;
	// 		newLen.normalize();
	// 		ngl::Vec3 dx = newLen - structuralSpringsRestingLength[j];
	// 		ngl::Vec3 df = -ks * dx * (axis / newLen); // normalized direction.
	// 		force_per_vert[endPoint] += df;
	// 		force_per_vert[startPoint] -= df; // opposite direction.
	// 	}

	// 	// for (int j = 0; j < m_bendSprings.size(); ++j)
	// 	// {
	// 	// 	int startPoint = m_bendSprings[j].startPoint;
	// 	// 	int endPoint = m_bendSprings[j].endPoint;
	// 	// 	ngl::Vec3 axis = massPtsCurr[endPoint] - massPtsCurr[startPoint];
	// 	// 	ngl::Vec3 newLen = axis;
	// 	// 	newLen.normalize();
	// 	// 	ngl::Vec3 dx = newLen - structuralSpringsRestingLength[j];
	// 	// 	ngl::Vec3 df = -ks * dx * (axis / newLen); // normalized direction.
	// 	// 	force_per_vert[endPoint] += df;
	// 	// 	force_per_vert[startPoint] -= df; // opposite direction.
	// 	// }

	// 	// for (int j = 0; j < m_shearSprings.size(); ++j)
	// 	// {
	// 	// 	int startPoint = m_shearSprings[j].startPoint;
	// 	// 	int endPoint = m_shearSprings[j].endPoint;
	// 	// 	ngl::Vec3 axis = massPtsCurr[endPoint] - massPtsCurr[startPoint];
	// 	// 	ngl::Vec3 newLen = axis;
	// 	// 	newLen.normalize();
	// 	// 	ngl::Vec3 dx = newLen - structuralSpringsRestingLength[j];
	// 	// 	ngl::Vec3 df = -ks * dx * (axis / newLen); // normalized direction.
	// 	// 	force_per_vert[endPoint] += df;
	// 	// 	force_per_vert[startPoint] -= df; // opposite direction.
	// 	// }

	// 	// apply external forces
	// 	// force_per_vert[0] += ngl::Vec3(0.0, 100.0, 0.0);

	// 	// std::cout<<force_per_vert[20].m_x<<", "<<force_per_vert[20].m_y<<", "<<force_per_vert[20].m_z<<"\n";

	// 	auto velNext = velCurr;
	// 	auto ptsNext = massPtsCurr;

	// 	for (int k = 0; k < force_per_vert.size(); ++k)
	// 	{
	// 		velNext[k] = velCurr[k] + dt * (force_per_vert[k] / m);
	// 		ptsNext[k] = massPtsCurr[k] + dt * velNext[k];
	// 		// m_massPts[k] = massPtsCurr[k] + dt * velNext[k];
	// 	}

	// 	// update current state.
	// 	velCurr = velNext;
	// 	massPtsCurr = ptsNext;

	}

	// this works
	m_massPts = massPtsCurr;*/
// }

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
