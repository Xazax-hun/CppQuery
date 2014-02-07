#include "query_widget.h"

#include "queryhighlighter.h"
#include "autocompleter_textedit.h"

QueryWidget::QueryWidget(QWidget* parent) :
	QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout;

	queryText = new AutoCompleterTextEdit;
	execute = new QPushButton(tr("Execute!"));

	layout->addWidget(queryText);
	layout->addWidget(execute);

	setLayout(layout);

	highlighter = new QueryHighlighter(queryText->document());

	connect(execute, &QPushButton::clicked, this, &QueryWidget::executeButtonPressed);

	// Autocompletion support
	QStringList matchers;
	matchers <<
		#include "matchers.txt"
	;

	QCompleter* completer = new QCompleter(matchers, this);
	completer->setCaseSensitivity(Qt::CaseSensitive);
	queryText->setCompleter(completer);
}

void QueryWidget::executeButtonPressed()
{
	std::string query = queryText->toPlainText().toStdString();

	emit executeQuery(query);
}
