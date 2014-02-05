#ifndef __QUERY_WIDGET_H__
#define __QUERY_WIDGET_H__

#include <QtWidgets>

class QueryWidget : public QWidget {
	Q_OBJECT

public:
	QueryWidget(QWidget* parent = nullptr);

public slots:
	void executeButtonPressed();

signals:
	void executeQuery(const std::string& query);

private:
	QTextEdit* queryText;
	QPushButton* execute;
};

#endif
