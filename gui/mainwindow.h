#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <memory>

#include <QtWidgets>

class Session;
class QueryWidget;
class CodeViewArea;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	void executeQuery(const std::string& query);
	void openResult(const QModelIndex& index);

private slots:
	void about();

private:
	void createMenuBar();
	void openMatcherReference();
	void open();

	QTableView* searchResults;

	CodeViewArea* resultText;
	QueryWidget* queryWidget;

	QMenu* fileMenu;
	QMenu* helpMenu;

	QAction* openAct;
	QAction* exitAct;

	QAction* matcherHelpAct;
	QAction* aboutAct;
	QAction* aboutQtAct;

	std::unique_ptr<Session> session;
};

#endif // __MAINWINDOW_H__
