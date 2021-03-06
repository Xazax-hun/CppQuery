#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <set>
#include <string>
#include <memory>
#include <utility>

#include <QStringList>
#include <QtWidgets>
#include <QThread>

#include "util.h"

class QSettings;

namespace CppQuery {

class Session;
class QueryWidget;
class CodeViewArea;
class QueryCatalogWindow;
class SearchResults;

/// \brief Worker thread that parses the C++ files.
///
/// This class represents a thread that is responsible for parsing the C++
/// files. It emits a signal after each TU is done to notify a progress bar. It
/// also emits a signal when the parsing is done. It requires a session to work
/// with, it does not own the session.
class ParserWorker : public QThread {
  Q_OBJECT

signals:
  void filesDone(int);
  void parseDone();
  void parseFail(const QString);

public:
  ParserWorker(QWidget *parent);
  ~ParserWorker();

  /// \brief Set the session that is used for parsing.
  ///
  /// Set the session that is used for parsing. It does not own the session. If
  /// the session is nullptr, the thread will not parse anything.
  void setSession(Session *session);


  /// \brief Set the source files that are parsed.
  ///
  /// The list of source files with full path that should be parsed. All of the
  /// files should be included in the compilation database of the session as
  /// well.
  void setSources(const QStringList &sourceFiles);

protected:
  void run() override;

private:
  void emitFilesDone(int i) { emit filesDone(i); }

  Session *session;
  std::set<std::string> sources;
};

/// \brief The main window of CppQuery.
///
/// This class is instantiated once for every instance of the application. It
/// owns every GUI element the application displays. It also have a QThread
/// object that represents a worker thread for parsing the C++ files. It owns
/// the session that is used to maintain the list of TUs and ASTs, and run the
/// queries.
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

public slots:
  /// \brief Execute a query.
  ///
  /// Take a string and pass it to the session to interpret and execute it as a
  /// query. If the query fails, it will show an error message.
  void executeQuery(const std::string &query);

  /// Save a query to the query catalog.
  void saveQuery(const std::string &query);

  /// \brief Open a location found by a query.
  ///
  /// It opens a file and jumps to the location that was found by a query. It
  /// also highlights the source range that is represented by the AST node that
  /// was found.
  void openResult(const QModelIndex &index);

private slots:
  void about();

private:
  /// Create menu bars for the main window.
  void createMenuBar();

  /// Opens the ASTMatcher reference page in a browser.
  void openMatcherReference();

  /// Opens a json compilation database
  void open();

  /// Handle parsing failures
  void onParseFail(const QString reason);

  /// Handle parsing done event
  void onParseDone();

  /// Open the query catalog 
  void openQueryCatalog();

  /// Export query results to a text file format
  void exportQueryResults();

  SearchResults *searchResults;

  CodeViewArea *resultText;
  QueryWidget *queryWidget;
  QueryCatalogWindow *catalogWindow;

  QMenu *fileMenu;
  QMenu *helpMenu;

  QAction *openAct;
  QAction *openQueryCatalogAct;
  QAction *exportResultsAct;
  QAction *exitAct;

  QAction *matcherHelpAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QSettings *settings;

  ParserWorker *parser;
  QProgressDialog *parseProgress;

  OwningPtr<Session> session;
};
}

#endif // __MAINWINDOW_H__
