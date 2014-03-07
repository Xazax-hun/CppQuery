#ifndef __QUERY_WIDGET_H__
#define __QUERY_WIDGET_H__

#include <QtWidgets>

class QueryHighlighter;
class AutoCompleterTextEdit;

class QueryWidget : public QWidget {
  Q_OBJECT

public:
  QueryWidget(QWidget *parent = nullptr);

public
slots:
  void executeButtonPressed();

signals:
  void executeQuery(const std::string &query);

private:
  AutoCompleterTextEdit *queryText;
  QPushButton *execute;
  QueryHighlighter *highlighter;
};

#endif
