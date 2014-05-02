#include "mainwindow.h"

#include <QSettings>
#include <QDesktopServices>
#include <QJsonDocument>

#include "query_widget.h"
#include "codeview_widget.h"
#include "querycatalog_window.h"
#include "fileselector_window.h"
#include "searchresults_widget.h"

#include "session.h"

using namespace CppQuery;

MainWindow::MainWindow() {
  setWindowTitle(tr("CppQuery"));
  resize(1000, 700);

  catalogWindow = new QueryCatalogWindow(this);
  settings = new QSettings("settings.ini", QSettings::IniFormat, this);

  QDockWidget *searchResultDock = new QDockWidget(tr("Search Results"), this);
  QDockWidget *queryTextDock = new QDockWidget(tr("Query"), this);

  searchResultDock->setFeatures(QDockWidget::DockWidgetMovable |
                                QDockWidget::DockWidgetFloatable);
  queryTextDock->setFeatures(QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);

  queryTextDock->setMaximumHeight(150);
  searchResultDock->setMinimumWidth(380);

  searchResults = new SearchResults(searchResultDock);
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

  parser = new ParserWorker(this);
  parseProgress =
      new QProgressDialog(tr("Parsing files..."), tr("Cancel"), 0, 0, this);
  parseProgress->setWindowModality(Qt::WindowModal);
  parseProgress->hide();

  connect(queryWidget, &QueryWidget::executeQuery, this,
          &MainWindow::executeQuery);
  connect(queryWidget, &QueryWidget::saveQuery, this,
          &MainWindow::saveQuery);
  connect(searchResults, &SearchResults::doubleClicked, this,
          &MainWindow::openResult);
  connect(catalogWindow, &QueryCatalogWindow::doubleClicked,
          [this](const QModelIndex &index) {
    QModelIndex realIndex(index.sibling(index.row(), 2));
    queryWidget->setQueryText(realIndex.data().toString().toStdString());
  });

  // Connect the options to the worker thread;
  connect(parser, &ParserWorker::filesDone,
          [this](int i) { parseProgress->setValue(i); });

  connect(parser, &ParserWorker::parseDone, this, &MainWindow::onParseDone,
          Qt::QueuedConnection);


  connect(parser, &ParserWorker::parseFail, this, &MainWindow::onParseFail,
          Qt::QueuedConnection);

  // Cancel button terminates the parser thread
  connect(parseProgress, &QProgressDialog::canceled, [this]() {
    parser->terminate();
    session.reset(nullptr);
  });

  statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {
  delete searchResults->model();
  delete parser;
}

void MainWindow::createMenuBar() {
  openAct = new QAction(tr("&Open..."), this);
  openQueryCatalogAct = new QAction(tr("Query catalog"), this);
  exportResultsAct = new QAction(tr("Export results..."), this);
  exitAct = new QAction(tr("E&xit"), this);
  matcherHelpAct = new QAction(tr("Matcher reference"), this);
  aboutAct = new QAction(tr("About"), this);
  aboutQtAct = new QAction(tr("About Qt"), this);

  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open a JSON compilation database."));

  exportResultsAct->setShortcuts(QKeySequence::SaveAs);

  exitAct->setShortcuts(QKeySequence::Quit);

  connect(openAct, &QAction::triggered, this, &MainWindow::open);
  connect(openQueryCatalogAct, &QAction::triggered, this,
          &MainWindow::openQueryCatalog);
  connect(exportResultsAct, &QAction::triggered, this,
          &MainWindow::exportQueryResults);
  connect(exitAct, &QAction::triggered, this, &MainWindow::close);
  connect(matcherHelpAct, &QAction::triggered, this,
          &MainWindow::openMatcherReference);
  connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
  connect(aboutQtAct, &QAction::triggered, this, &QApplication::aboutQt);

  fileMenu = menuBar()->addMenu(tr("&File"));
  helpMenu = menuBar()->addMenu(tr("&Help"));

  fileMenu->addAction(openAct);
  fileMenu->addAction(openQueryCatalogAct);
  fileMenu->addAction(exportResultsAct);
  fileMenu->addAction(exitAct);

  helpMenu->addAction(matcherHelpAct);
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::openQueryCatalog() {
  catalogWindow->readFromFile(
      settings->value("settings/catalog").toString().toStdString());
  catalogWindow->show();
}

void MainWindow::saveQuery(const std::string& query) {
  catalogWindow->readFromFile(
      settings->value("settings/catalog").toString().toStdString());
  catalogWindow->addQuery(query);
  catalogWindow->show();
}

void MainWindow::onParseDone() {
  parseProgress->hide();
  statusBar()->showMessage(tr("Parsing done..."));
}

void MainWindow::onParseFail(const QString reason) {
  QMessageBox::critical(this, tr("Error"),
                        tr("Unable to parse C++ code:\n") + reason);
  session.reset(nullptr);
  statusBar()->showMessage(tr("Parsing is failed..."));
}

void MainWindow::open() {
  std::string fileName =
      QFileDialog::getOpenFileName(this, tr("Open Compilation Database"), "",
                                   tr("JSON files (*.json)")).toStdString();

  if (fileName.empty())
    return;

  try {
    session.reset(new Session(
        fileName,
        settings->value("clang/resource-dir").toString().toStdString()));

    QStringList sourceFiles;

    QFile file(fileName.c_str());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError error;
    QJsonDocument document =
        QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error == QJsonParseError::NoError && document.isArray()) {
      QJsonArray array = document.array();
      for (QJsonValue val : array) {
        if (!val.isObject())
          continue;
        QJsonObject obj = val.toObject();
        QJsonValue fileName = obj.value("file");
        sourceFiles << fileName.toString();
      }
    }

    FileSelectorDialog d(sourceFiles, this);
    d.exec();

    parseProgress->show();
    parseProgress->setMaximum(d.getSelectedFiles().size());
    parseProgress->setValue(0);

    parser->setSession(session.get());
    parser->setSources(d.getSelectedFiles());
    parser->start();

    // Remove the results of the session that was open earlier
    QAbstractItemModel *model = searchResults->model();
    model->removeRows(0, model->rowCount());
    searchResults->setModel(model);
  }
  catch (DatabaseError &e) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to open compilation database: ") +
                              QString::fromStdString(e.getReason()));
    session.reset(nullptr);
  }
}

void MainWindow::exportQueryResults() {
  std::string fileName =
      QFileDialog::getSaveFileName(this, tr("Export query results"), "",
                                   tr("Text files (*.txt)")).toStdString();
  if (fileName.empty())
    return;

  // TODO: consider filtering
  session->exportMatches(fileName);

  statusBar()->showMessage(tr("Results are exported..."));
}

void MainWindow::about() {
  QMessageBox::about(
      this, tr("About Menu"),
      tr("A query language based on Clang's ASTMatcher library, to provide the "
         "programmers with a more efficient way to navigate in huge codebases "
         "than simple text searches."));
}

void MainWindow::openMatcherReference() {
  QDesktopServices::openUrl(
      QUrl("http://clang.llvm.org/docs/LibASTMatchersReference.html"));
}

void MainWindow::executeQuery(const std::string &query) {
  if (session) {
    statusBar()->showMessage(tr("Querying..."));

    try {
      session->runQuery(query);
    }
    catch (QueryError &e) {
      QMessageBox::critical(this, tr("Error"),
                            tr("Unable to parse the query: ") +
                                QString::fromStdString(e.getReason()));
      statusBar()->showMessage(tr("Query failed."), 3);
      return;
    }

    QAbstractItemModel *model = searchResults->model();
    model->removeRows(0, model->rowCount());

    auto matches = session->getMatches();

    unsigned max = matches.size();
    model->insertRows(0, max);

    unsigned i = 0;
    for (const Match &m : matches) {
      model->setData(model->index(i, 0), QVariant(QString::fromStdString(m.id)),
                     Qt::DisplayRole);
      model->setData(model->index(i, 1),
                     QVariant(QString::fromStdString(m.fileName)),
                     Qt::DisplayRole);
      model->setData(model->index(i, 2), QVariant(m.startLine),
                     Qt::DisplayRole);
      model->setData(model->index(i, 3), QVariant(m.startCol), Qt::DisplayRole);
      model->setData(model->index(i, 4), QVariant(m.endLine), Qt::DisplayRole);
      model->setData(model->index(i, 5), QVariant(m.endCol), Qt::DisplayRole);
      ++i;
    }

    searchResults->setModel(model);

    statusBar()->showMessage(tr("Ready"));
  }
}

void MainWindow::openResult(const QModelIndex &index) {
  int row = index.row();

  QAbstractItemModel *model = searchResults->model();

  QString id = model->data(model->index(row, 0)).toString();
  QString fileName = model->data(model->index(row, 1)).toString();
  int startLine = model->data(model->index(row, 2)).toInt();
  int startCol = model->data(model->index(row, 3)).toInt();
  int endLine = model->data(model->index(row, 4)).toInt();
  int endCol = model->data(model->index(row, 5)).toInt();

  QFile file{ fileName };

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    statusBar()->showMessage(tr("Unable to open the location..."));
    return;
  }

  QTextStream reader(&file);

  resultText->setPlainText(reader.readAll());

  resultText->highlightArea(startLine, startCol, endLine, endCol);
  statusBar()->showMessage(tr("Ready"));
}

ParserWorker::ParserWorker(QWidget *parent)
    : QThread(parent), session(nullptr) {}

ParserWorker::~ParserWorker() {}

void ParserWorker::setSession(Session *session) { this->session = session; }

void ParserWorker::setSources(const QStringList &sourceFiles) {
  for (const QString &str : sourceFiles) {
  	sources.insert(str.toStdString());
  }
}

void ParserWorker::run() {
  if (!session)
    return;

  try {
    int i = 0;
    session->parseFiles([this](const std::string &TUName) {
                          return sources.count(TUName);
                        },
    [&i, this ](const std::string & TUName)->bool {
      emitFilesDone(++i);
      return true;
    });
  }
  catch (ParseError &e) {
    emit parseFail(QString::fromStdString(e.getReason()));
  }

  emit parseDone();
}
