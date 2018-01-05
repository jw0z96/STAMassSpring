#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_jelloCube = new JelloCube(m_ui->springConstantSpinBox->value(), m_ui->dampingConstantSpinBox->value());
	m_gl = new NGLScene(this, m_jelloCube);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);

	connect(m_ui->displayMassesCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleMassPoints()));
	connect(m_ui->displayStructuralSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleStructuralSpring()));
	connect(m_ui->displayShearSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleShearSpring()));
	connect(m_ui->displayBendSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleBendSpring()));

	connect(m_ui->springConstantSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setK(double)));
	connect(m_ui->dampingConstantSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setDamping(double)));
	connect(m_ui->timeStepSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setTimeStep(double)));
	connect(m_ui->resetJelloPushButton, SIGNAL(released()), m_jelloCube, SLOT(reset()));
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_gl;
	delete m_jelloCube;
}
