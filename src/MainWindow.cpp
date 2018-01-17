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

	// reset button
	connect(m_ui->resetJelloPushButton, SIGNAL(released()), this, SLOT(resetJelloCube()));

	// display controls
	connect(m_ui->displayMeshCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleMesh()));
	connect(m_ui->displayMassesCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleMassPoints()));
	connect(m_ui->displayStructuralSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleStructuralSpring()));
	connect(m_ui->displayShearSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleShearSpring()));
	connect(m_ui->displayBendSpringsCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleBendSpring()));

	// physics controls
	connect(m_ui->enableSphereColliderCheckBox, SIGNAL(stateChanged(int)), this, SLOT(toggleSphereCollider()));
	connect(m_ui->springConstantSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setK(double)));
	connect(m_ui->dampingConstantSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setDamping(double)));
	connect(m_ui->jelloMassSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setMass(double)));
	connect(m_ui->gravitySpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setGravity(double)));
	// timestep
	connect(m_ui->timeStepSpinBox,SIGNAL(valueChanged(double)), m_jelloCube,SLOT(setTimeStep(double)));
	// sub steps spin box
	connect(m_ui->subStepsSpinBox, SIGNAL(valueChanged(int)), m_jelloCube, SLOT(setSubSteps(int)));

	// populate integration scheme combobox
	QList<QString> integratorsList;
	integratorsList.append("RK4");
	integratorsList.append("RK2");
	integratorsList.append("Euler");
	m_ui->integratorComboBox->addItems(integratorsList);
	connect(m_ui->integratorComboBox, SIGNAL(currentIndexChanged(int)), m_jelloCube, SLOT(setIntegrator(int)));
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_gl;
	delete m_jelloCube;
}

void MainWindow::resetJelloCube()
{
	m_jelloCube->reset();
	updateUIText();
}

void MainWindow::toggleSphereCollider()
{
	m_jelloCube->toggleCollider();
	m_gl->toggleCollider();
}

void MainWindow::updateUIText()
{
	m_ui->massCountLabel->setText(QStringLiteral("Mass Count: %1").arg(m_jelloCube->getMassCount()));
	m_ui->springCountLabel->setText(QStringLiteral("Spring Count: %1").arg(m_jelloCube->getSpringCount()));
}
