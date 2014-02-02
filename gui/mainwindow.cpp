#include "mainwindow.h"

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

    exitAct = new QAction(tr("E&xit"), this);
    aboutAct = new QAction(tr("About"), this);
    aboutQtAct = new QAction(tr("About Qt"), this);

    exitAct->setShortcuts(QKeySequence::Quit);

    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    connect(aboutQtAct, &QAction::triggered, this, &QApplication::aboutQt);

	fileMenu = menuBar()->addMenu(tr("&File"));
	helpMenu = menuBar()->addMenu(tr("&Help"));

	fileMenu->addAction(exitAct);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);

	statusBar()->showMessage(tr("Ready"));
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Menu"),
            tr("A query language based on Clang's ASTMatcher library, to provide the programmers with a more efficient way to navigate in huge codebases than simple text searches."));
}