/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *s_mainWindowGridLayout;
    QVBoxLayout *verticalLayout;
    QGroupBox *displayControlsGB;
    QGridLayout *gridLayout_2;
    QCheckBox *displayShearSpringsCheckBox;
    QCheckBox *displayStructuralSpringsCheckBox;
    QCheckBox *displayBendSpringsCheckBox;
    QCheckBox *displayMassesCheckBox;
    QCheckBox *displayMeshCheckBox;
    QGroupBox *physicsControlsGB;
    QGridLayout *gridLayout_3;
    QPushButton *resetJelloPushButton;
    QFormLayout *formLayout;
    QLabel *springConstantLabel;
    QDoubleSpinBox *springConstantSpinBox;
    QLabel *dampingConstantLabel;
    QDoubleSpinBox *dampingConstantSpinBox;
    QLabel *jelloMassLabel;
    QDoubleSpinBox *jelloMassSpinBox;
    QLabel *gravityLabel;
    QDoubleSpinBox *gravitySpinBox;
    QLabel *timeStepLabel;
    QDoubleSpinBox *timeStepSpinBox;
    QLabel *subStepsLabel;
    QSpinBox *subStepsSpinBox;
    QComboBox *integratorComboBox;
    QLabel *integratorLabel;
    QLabel *massCountLabel;
    QLabel *springCountLabel;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1135, 741);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        s_mainWindowGridLayout = new QGridLayout(centralwidget);
        s_mainWindowGridLayout->setObjectName(QStringLiteral("s_mainWindowGridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        displayControlsGB = new QGroupBox(centralwidget);
        displayControlsGB->setObjectName(QStringLiteral("displayControlsGB"));
        gridLayout_2 = new QGridLayout(displayControlsGB);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        displayShearSpringsCheckBox = new QCheckBox(displayControlsGB);
        displayShearSpringsCheckBox->setObjectName(QStringLiteral("displayShearSpringsCheckBox"));

        gridLayout_2->addWidget(displayShearSpringsCheckBox, 4, 0, 1, 1);

        displayStructuralSpringsCheckBox = new QCheckBox(displayControlsGB);
        displayStructuralSpringsCheckBox->setObjectName(QStringLiteral("displayStructuralSpringsCheckBox"));

        gridLayout_2->addWidget(displayStructuralSpringsCheckBox, 3, 0, 1, 1);

        displayBendSpringsCheckBox = new QCheckBox(displayControlsGB);
        displayBendSpringsCheckBox->setObjectName(QStringLiteral("displayBendSpringsCheckBox"));

        gridLayout_2->addWidget(displayBendSpringsCheckBox, 5, 0, 1, 1);

        displayMassesCheckBox = new QCheckBox(displayControlsGB);
        displayMassesCheckBox->setObjectName(QStringLiteral("displayMassesCheckBox"));
        displayMassesCheckBox->setChecked(false);

        gridLayout_2->addWidget(displayMassesCheckBox, 1, 0, 1, 1);

        displayMeshCheckBox = new QCheckBox(displayControlsGB);
        displayMeshCheckBox->setObjectName(QStringLiteral("displayMeshCheckBox"));
        displayMeshCheckBox->setChecked(true);

        gridLayout_2->addWidget(displayMeshCheckBox, 0, 0, 1, 1);


        verticalLayout->addWidget(displayControlsGB);

        physicsControlsGB = new QGroupBox(centralwidget);
        physicsControlsGB->setObjectName(QStringLiteral("physicsControlsGB"));
        gridLayout_3 = new QGridLayout(physicsControlsGB);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        resetJelloPushButton = new QPushButton(physicsControlsGB);
        resetJelloPushButton->setObjectName(QStringLiteral("resetJelloPushButton"));

        gridLayout_3->addWidget(resetJelloPushButton, 7, 0, 1, 1);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        springConstantLabel = new QLabel(physicsControlsGB);
        springConstantLabel->setObjectName(QStringLiteral("springConstantLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, springConstantLabel);

        springConstantSpinBox = new QDoubleSpinBox(physicsControlsGB);
        springConstantSpinBox->setObjectName(QStringLiteral("springConstantSpinBox"));
        springConstantSpinBox->setDecimals(3);
        springConstantSpinBox->setSingleStep(0.1);
        springConstantSpinBox->setValue(1);

        formLayout->setWidget(1, QFormLayout::FieldRole, springConstantSpinBox);

        dampingConstantLabel = new QLabel(physicsControlsGB);
        dampingConstantLabel->setObjectName(QStringLiteral("dampingConstantLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, dampingConstantLabel);

        dampingConstantSpinBox = new QDoubleSpinBox(physicsControlsGB);
        dampingConstantSpinBox->setObjectName(QStringLiteral("dampingConstantSpinBox"));
        dampingConstantSpinBox->setDecimals(3);
        dampingConstantSpinBox->setSingleStep(0.1);
        dampingConstantSpinBox->setValue(2);

        formLayout->setWidget(2, QFormLayout::FieldRole, dampingConstantSpinBox);

        jelloMassLabel = new QLabel(physicsControlsGB);
        jelloMassLabel->setObjectName(QStringLiteral("jelloMassLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, jelloMassLabel);

        jelloMassSpinBox = new QDoubleSpinBox(physicsControlsGB);
        jelloMassSpinBox->setObjectName(QStringLiteral("jelloMassSpinBox"));
        jelloMassSpinBox->setDecimals(2);
        jelloMassSpinBox->setSingleStep(0.1);
        jelloMassSpinBox->setValue(1);

        formLayout->setWidget(3, QFormLayout::FieldRole, jelloMassSpinBox);

        gravityLabel = new QLabel(physicsControlsGB);
        gravityLabel->setObjectName(QStringLiteral("gravityLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, gravityLabel);

        gravitySpinBox = new QDoubleSpinBox(physicsControlsGB);
        gravitySpinBox->setObjectName(QStringLiteral("gravitySpinBox"));
        gravitySpinBox->setDecimals(2);
        gravitySpinBox->setMinimum(-99.99);
        gravitySpinBox->setSingleStep(0.1);
        gravitySpinBox->setValue(-9.81);

        formLayout->setWidget(4, QFormLayout::FieldRole, gravitySpinBox);

        timeStepLabel = new QLabel(physicsControlsGB);
        timeStepLabel->setObjectName(QStringLiteral("timeStepLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, timeStepLabel);

        timeStepSpinBox = new QDoubleSpinBox(physicsControlsGB);
        timeStepSpinBox->setObjectName(QStringLiteral("timeStepSpinBox"));
        timeStepSpinBox->setDecimals(4);
        timeStepSpinBox->setSingleStep(0.001);
        timeStepSpinBox->setValue(0.01);

        formLayout->setWidget(5, QFormLayout::FieldRole, timeStepSpinBox);

        subStepsLabel = new QLabel(physicsControlsGB);
        subStepsLabel->setObjectName(QStringLiteral("subStepsLabel"));

        formLayout->setWidget(6, QFormLayout::LabelRole, subStepsLabel);

        subStepsSpinBox = new QSpinBox(physicsControlsGB);
        subStepsSpinBox->setObjectName(QStringLiteral("subStepsSpinBox"));
        subStepsSpinBox->setMinimum(1);
        subStepsSpinBox->setMaximum(999);
        subStepsSpinBox->setValue(10);

        formLayout->setWidget(6, QFormLayout::FieldRole, subStepsSpinBox);

        integratorComboBox = new QComboBox(physicsControlsGB);
        integratorComboBox->setObjectName(QStringLiteral("integratorComboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, integratorComboBox);

        integratorLabel = new QLabel(physicsControlsGB);
        integratorLabel->setObjectName(QStringLiteral("integratorLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, integratorLabel);


        gridLayout_3->addLayout(formLayout, 1, 0, 2, 1);

        massCountLabel = new QLabel(physicsControlsGB);
        massCountLabel->setObjectName(QStringLiteral("massCountLabel"));

        gridLayout_3->addWidget(massCountLabel, 4, 0, 1, 1);

        springCountLabel = new QLabel(physicsControlsGB);
        springCountLabel->setObjectName(QStringLiteral("springCountLabel"));

        gridLayout_3->addWidget(springCountLabel, 5, 0, 1, 1);


        verticalLayout->addWidget(physicsControlsGB);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        s_mainWindowGridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1135, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "STA - Mass Spring Demo", Q_NULLPTR));
        displayControlsGB->setTitle(QApplication::translate("MainWindow", "Display", Q_NULLPTR));
        displayShearSpringsCheckBox->setText(QApplication::translate("MainWindow", "Shear Springs", Q_NULLPTR));
        displayStructuralSpringsCheckBox->setText(QApplication::translate("MainWindow", "Structural Springs", Q_NULLPTR));
        displayBendSpringsCheckBox->setText(QApplication::translate("MainWindow", "Bend Springs", Q_NULLPTR));
        displayMassesCheckBox->setText(QApplication::translate("MainWindow", "Masses", Q_NULLPTR));
        displayMeshCheckBox->setText(QApplication::translate("MainWindow", "Mesh", Q_NULLPTR));
        physicsControlsGB->setTitle(QApplication::translate("MainWindow", "Controls", Q_NULLPTR));
        resetJelloPushButton->setText(QApplication::translate("MainWindow", "Reset", Q_NULLPTR));
        springConstantLabel->setText(QApplication::translate("MainWindow", "Spring", Q_NULLPTR));
        dampingConstantLabel->setText(QApplication::translate("MainWindow", "Damping", Q_NULLPTR));
        jelloMassLabel->setText(QApplication::translate("MainWindow", "Jello Mass", Q_NULLPTR));
        gravityLabel->setText(QApplication::translate("MainWindow", "Gravity", Q_NULLPTR));
        timeStepLabel->setText(QApplication::translate("MainWindow", "Timestep", Q_NULLPTR));
        subStepsLabel->setText(QApplication::translate("MainWindow", "Substeps", Q_NULLPTR));
        integratorLabel->setText(QApplication::translate("MainWindow", "Integrator", Q_NULLPTR));
        massCountLabel->setText(QApplication::translate("MainWindow", "Mass Count: ?", Q_NULLPTR));
        springCountLabel->setText(QApplication::translate("MainWindow", "Spring Count: ?", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
