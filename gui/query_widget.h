#ifndef __QUERY_WIDGET_H__
#define __QUERY_WIDGET_H__

#include <QtWidgets>

namespace CppQuery {

class QueryHighlighter;
class AutoCompleterTextEdit;

/// \brief Holds query related buttons and widgets.
///
/// This widget is intended to hold every query related functionality. It is a
/// docked widget on the main window.
class QueryWidget : public QWidget {
  Q_OBJECT

public:
  QueryWidget(QWidget *parent = nullptr);

public slots:
  void executeButtonPressed();
  void saveButtonPressed();
  void setQueryText(const std::string &query);

signals:
  void executeQuery(const std::string &query);
  void saveQuery(const std::string &query);

private:
  AutoCompleterTextEdit *queryText;
  QPushButton *execute;
  QPushButton *save;
  QueryHighlighter *highlighter;
};
}

#endif
