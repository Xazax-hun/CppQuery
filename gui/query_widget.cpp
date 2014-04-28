#include "query_widget.h"

#include "queryhighlighter.h"
#include "autocompleter_textedit.h"
#include "query_completer.h"

using namespace CppQuery;

QueryWidget::QueryWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *vlayout = new QVBoxLayout;
  QHBoxLayout *hlayout = new QHBoxLayout;

  queryText = new AutoCompleterTextEdit;
  execute = new QPushButton(tr("Execute!"));
  save = new QPushButton(tr("Save to catalog"));

  hlayout->addWidget(save);
  hlayout->addWidget(execute);
  vlayout->addWidget(queryText);
  vlayout->addLayout(hlayout);

  setLayout(vlayout);

  highlighter = new QueryHighlighter(queryText->document());

  connect(execute, &QPushButton::clicked, this,
          &QueryWidget::executeButtonPressed);
  connect(save, &QPushButton::clicked, this, &QueryWidget::saveButtonPressed);

  // Autocompletion support
  //  QStringList matchers;
  //  matchers <<
  //#include "matchers.txt"
  //     ;

  // QCompleter *completer = new QCompleter(matchers, this);
  completer = new QueryCompleter(this);
  completer->setCaseSensitivity(Qt::CaseSensitive);
  queryText->setCompleter(completer);
}

void QueryWidget::setQueryText(const std::string &query) {
  queryText->setPlainText(QString::fromStdString(query));
}

void QueryWidget::executeButtonPressed() {
  std::string query = queryText->toPlainText().toStdString();

  emit executeQuery(query);
}

void QueryWidget::saveButtonPressed() {
  std::string query = queryText->toPlainText().toStdString();

  emit saveQuery(query);
}
