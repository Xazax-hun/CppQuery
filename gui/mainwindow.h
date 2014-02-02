#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtWidgets>

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();

private slots:
	void about();

private:
	QListWidget* searchResults;

	QTextEdit* resultText;
	QTextEdit* queryText;

	QMenu* fileMenu;
	QMenu* helpMenu;

	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;
};

#endif // __MAINWINDOW_H__
