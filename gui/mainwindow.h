#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <memory>

#include <QtWidgets>

class Session;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private slots:
	void about();

private:
	void createMenuBar();
	void open();

	QListWidget* searchResults;

	QTextEdit* resultText;
	QTextEdit* queryText;

	QMenu* fileMenu;
	QMenu* helpMenu;

	QAction *openAct;
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	std::unique_ptr<Session> session;
};

#endif // __MAINWINDOW_H__
