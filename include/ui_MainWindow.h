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
    QGroupBox *physicsControlsGB;
    QGridLayout *gridLayout_3;
    QFormLayout *formLayout;
    QDoubleSpinBox *springConstantSpinBox;
    QDoubleSpinBox *dampingConstantSpinBox;
    QLabel *springConstantLabel;
    QLabel *dampingConstantLabel;
    QPushButton *resetJelloPushButton;
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

        gridLayout_2->addWidget(displayShearSpringsCheckBox, 2, 0, 1, 1);

        displayStructuralSpringsCheckBox = new QCheckBox(displayControlsGB);
        displayStructuralSpringsCheckBox->setObjectName(QStringLiteral("displayStructuralSpringsCheckBox"));

        gridLayout_2->addWidget(displayStructuralSpringsCheckBox, 1, 0, 1, 1);

        displayBendSpringsCheckBox = new QCheckBox(displayControlsGB);
        displayBendSpringsCheckBox->setObjectName(QStringLiteral("displayBendSpringsCheckBox"));

        gridLayout_2->addWidget(displayBendSpringsCheckBox, 3, 0, 1, 1);

        displayMassesCheckBox = new QCheckBox(displayControlsGB);
        displayMassesCheckBox->setObjectName(QStringLiteral("displayMassesCheckBox"));
        displayMassesCheckBox->setChecked(true);

        gridLayout_2->addWidget(displayMassesCheckBox, 0, 0, 1, 1);


        verticalLayout->addWidget(displayControlsGB);

        physicsControlsGB = new QGroupBox(centralwidget);
        physicsControlsGB->setObjectName(QStringLiteral("physicsControlsGB"));
        gridLayout_3 = new QGridLayout(physicsControlsGB);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        springConstantSpinBox = new QDoubleSpinBox(physicsControlsGB);
        springConstantSpinBox->setObjectName(QStringLiteral("springConstantSpinBox"));
        springConstantSpinBox->setValue(5);

        formLayout->setWidget(0, QFormLayout::FieldRole, springConstantSpinBox);

        dampingConstantSpinBox = new QDoubleSpinBox(physicsControlsGB);
        dampingConstantSpinBox->setObjectName(QStringLiteral("dampingConstantSpinBox"));
        dampingConstantSpinBox->setValue(2);

        formLayout->setWidget(1, QFormLayout::FieldRole, dampingConstantSpinBox);

        springConstantLabel = new QLabel(physicsControlsGB);
        springConstantLabel->setObjectName(QStringLiteral("springConstantLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, springConstantLabel);

        dampingConstantLabel = new QLabel(physicsControlsGB);
        dampingConstantLabel->setObjectName(QStringLiteral("dampingConstantLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, dampingConstantLabel);


        gridLayout_3->addLayout(formLayout, 0, 0, 1, 1);

        resetJelloPushButton = new QPushButton(physicsControlsGB);
        resetJelloPushButton->setObjectName(QStringLiteral("resetJelloPushButton"));

        gridLayout_3->addWidget(resetJelloPushButton, 6, 0, 1, 1);


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
        physicsControlsGB->setTitle(QApplication::translate("MainWindow", "Controls", Q_NULLPTR));
        springConstantLabel->setText(QApplication::translate("MainWindow", "Spring", Q_NULLPTR));
        dampingConstantLabel->setText(QApplication::translate("MainWindow", "Damping", Q_NULLPTR));
        resetJelloPushButton->setText(QApplication::translate("MainWindow", "Reset", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
