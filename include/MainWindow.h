#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"
#include "JelloCube.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	// public slots:

	private:
		// QWidget *m_ui;
		Ui::MainWindow *m_ui;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief our openGL widget
		//----------------------------------------------------------------------------------------------------------------------
		NGLScene *m_gl;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief our jello cube
		//----------------------------------------------------------------------------------------------------------------------
		JelloCube *m_jelloCube;
};

#endif // MAINWINDOW_H
