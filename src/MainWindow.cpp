#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);

	connect(m_ui->displayMassesCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleMassPoints()));
	connect(m_ui->displayStructuralSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleStructuralSpring()));
	connect(m_ui->displayShearSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleShearSpring()));
	connect(m_ui->displayBendSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleBendSpring()));
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_gl;
}
