#include "mainwindow.h"

#include <string>

#include "session.h"

MainWindow::MainWindow() {
	setWindowTitle(tr("CppQuery"));

	QDockWidget *searchResultDock = new QDockWidget(tr("Search Results"), this);
	QDockWidget *queryTextDock = new QDockWidget(tr("Query"), this);

	searchResultDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	queryTextDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

	searchResults = new QListWidget(searchResultDock);
	queryText = new QTextEdit(queryTextDock);

	searchResultDock->setWidget(searchResults);
	queryTextDock->setWidget(queryText);

	addDockWidget(Qt::LeftDockWidgetArea, searchResultDock);
	addDockWidget(Qt::BottomDockWidgetArea, queryTextDock);

	resultText = new QTextEdit(this);
	setCentralWidget(resultText);
	resultText->setReadOnly(true);

	createMenuBar();

	statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {

}

void MainWindow::createMenuBar() {
	exitAct = new QAction(tr("E&xit"), this);
	aboutAct = new QAction(tr("About"), this);
	aboutQtAct = new QAction(tr("About Qt"), this);

	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open a JSON compilation database."));

	exitAct->setShortcuts(QKeySequence::Quit);

	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	connect(exitAct, &QAction::triggered, this, &MainWindow::close);
	connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
	connect(aboutQtAct, &QAction::triggered, this, &QApplication::aboutQt);

	fileMenu = menuBar()->addMenu(tr("&File"));
	helpMenu = menuBar()->addMenu(tr("&Help"));

	fileMenu->addAction(openAct);
	fileMenu->addAction(exitAct);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::open() {
	std::string fileName = QFileDialog::getOpenFileName(this,
    	tr("Open Compilation Database"), "", tr("JSON files (*.json)")).toStdString();

	// TODO: progressbar

	session = std::unique_ptr<Session>(new Session(fileName));
}


void MainWindow::about() {
    QMessageBox::about(this, tr("About Menu"),
            tr("A query language based on Clang's ASTMatcher library, to provide the programmers with a more efficient way to navigate in huge codebases than simple text searches."));
}
