#include "query_widget.h"

QueryWidget::QueryWidget(QWidget* parent) :
	QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout;

	queryText = new QTextEdit;
	execute = new QPushButton(tr("Execute!"));

	layout->addWidget(queryText);
	layout->addWidget(execute);

	setLayout(layout);

	connect(execute, &QPushButton::clicked, this, &QueryWidget::executeButtonPressed);
}

void QueryWidget::executeButtonPressed()
{
	std::string query = queryText->toPlainText().toStdString();

	emit executeQuery(query);
}
