#include "mainwindow.h"

#include <string>

#include <QRegularExpression>
#include <QDesktopServices>

#include "query_widget.h"
#include "codeview_widget.h"
#include "session.h"

MainWindow::MainWindow() {
	setWindowTitle(tr("CppQuery"));

	QDockWidget *searchResultDock = new QDockWidget(tr("Search Results"), this);
	QDockWidget *queryTextDock = new QDockWidget(tr("Query"), this);

	searchResultDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	queryTextDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

	searchResults = new QListWidget(searchResultDock);
	queryWidget = new QueryWidget(queryTextDock);

	searchResultDock->setWidget(searchResults);
	queryTextDock->setWidget(queryWidget);

	addDockWidget(Qt::LeftDockWidgetArea, searchResultDock);
	addDockWidget(Qt::BottomDockWidgetArea, queryTextDock);

	resultText = new CodeViewArea(this);
	setCentralWidget(resultText);
	resultText->setReadOnly(true);
	resultText->setLineWrapMode(QPlainTextEdit::NoWrap);

	createMenuBar();

	statusBar()->showMessage(tr("Ready"));

	connect(queryWidget, &QueryWidget::executeQuery, this, &MainWindow::executeQuery);
	connect(searchResults, &QListWidget::itemDoubleClicked, this, &MainWindow::openResult);
}

MainWindow::~MainWindow() {

}

void MainWindow::createMenuBar() {
	openAct = new QAction(tr("&Open..."), this);
	exitAct = new QAction(tr("E&xit"), this);
	matcherHelpAct = new QAction(tr("Matcher reference"), this);
	aboutAct = new QAction(tr("About"), this);
	aboutQtAct = new QAction(tr("About Qt"), this);

	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open a JSON compilation database."));

	exitAct->setShortcuts(QKeySequence::Quit);

	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	connect(exitAct, &QAction::triggered, this, &MainWindow::close);
	connect(matcherHelpAct, &QAction::triggered, this, &MainWindow::openMatcherReference);
	connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
	connect(aboutQtAct, &QAction::triggered, this, &QApplication::aboutQt);

	fileMenu = menuBar()->addMenu(tr("&File"));
	helpMenu = menuBar()->addMenu(tr("&Help"));

	fileMenu->addAction(openAct);
	fileMenu->addAction(exitAct);

	helpMenu->addAction(matcherHelpAct);
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

void MainWindow::openMatcherReference() {
	QDesktopServices::openUrl(QUrl("http://clang.llvm.org/docs/LibASTMatchersReference.html"));
}

void MainWindow::executeQuery(const std::string& query) {
	if (session) {
		statusBar()->showMessage(tr("Querying..."));

		session->runQuery(query);

		searchResults->clear();
		
		auto matches = session->getMatches();
		
		for( const std::string& entry : matches ) {
			searchResults->addItem(QString::fromStdString(entry));
		}

		statusBar()->showMessage(tr("Ready"));
	}
}

void MainWindow::openResult(QListWidgetItem* item) {
	QString content = item->text();
	QRegularExpression re{R"((.+) \((.+)\): (\d+),\d+)"};
	QRegularExpressionMatch match = re.match(content);

	QString fileName = match.captured(1);
	QString id = match.captured(2);
	unsigned line = match.captured(3).toUInt();

	QFile file{fileName};

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert(false && "Unhandled error");
	}

	QTextStream reader(&file);

	resultText->setPlainText(reader.readAll());

	QTextCursor cursor = resultText->textCursor();
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
	resultText->setTextCursor(cursor);
}
