#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <memory>
#include <utility>

#include <QtWidgets>
#include <QThread>

class Session;
class QueryWidget;
class CodeViewArea;

class ParserWorker : public QThread {
	Q_OBJECT

signals:
	void filesDone(int);
	void parseDone(std::unique_ptr<Session>*);

public:
	ParserWorker(QWidget* parent);
	~ParserWorker();

	void setSession(std::unique_ptr<Session> session);
	
protected:
	void run() override;

private:
	void emitFilesDone(int i) { emit filesDone(i); }

	std::unique_ptr<Session> session;
};

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

	ParserWorker* parser;
	QProgressDialog* parseProgress;

	std::unique_ptr<Session> session;
};

#endif // __MAINWINDOW_H__
