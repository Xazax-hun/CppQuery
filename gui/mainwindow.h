#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <memory>

#include <QtWidgets>

class Session;
class QueryWidget;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	void executeQuery(const std::string& query);

private slots:
	void about();

private:
	void createMenuBar();
	void open();

	QListWidget* searchResults;

	QTextEdit* resultText;
	QueryWidget* queryWidget;

	QMenu* fileMenu;
	QMenu* helpMenu;

	QAction *openAct;
	QAction *exitAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	std::unique_ptr<Session> session;
};

#endif // __MAINWINDOW_H__
